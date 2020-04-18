#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanBarracks.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"

HumanBarracks::HumanBarracks(int posx, int posy) : StaticEnt(StaticEntType::HumanBarracks)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 40;
	collrange = 25;
	selectable = true;
	isSelected = false;
	to_delete = false;
	canbuild = false;
	create_swordman = false;
	time_bar_start = false;
	//selectable_buildings = true;
	construction_time = 3;
	time_FX_barracks = 1;
	first_upgrade_time = 10;
	timer_queue = 0;
	troop_type = 0;
	pos0 = { 827, 103 };
	pos1 = { 890, 103 };
	pos2 = { 827, 168 };
	pos3 = { 890, 168 };
	pos4 = { 827, 230 };
	pos5 = { 890, 230 };
	// Load all animations
	inconstruction.PushBack({ 399,410,96,81 }, 0.2, 0, 0, 0, 0);
	finishedconst.PushBack({ 403,273,96,95 }, 0.2, 0, 0, 0, 0);
	team = TeamType::NO_TYPE;
	actualState = ST_BARRACK_PREVIEW;
	life_points = 100;
	createUI = false;
	Barrack_Upgraded = false;
}

HumanBarracks::~HumanBarracks()
{}

bool HumanBarracks::Start()
{
	if (App->scene->active == true) {
		actualState = ST_BARRACK_AUTOMATIC;
	}
	createUI = true;
	return true;
}

bool HumanBarracks::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		life_points = 0;
	}
	if (life_points <= 0)
	{
		if (Button_Create_Footman != nullptr)
		{
			DeleteBarracksUI();
		}
		for (int i = 0; i < Troop.size(); i++)
		{
			if (Troop[i]->image != nullptr)
			{
				Troop[i]->image->to_delete = true;
			}
			if (Troop[i]->bar != nullptr)
			{
				Troop[i]->bar->to_delete = true;
			}
		}
		if (creation_barrack_bar != nullptr) 
		{
			creation_barrack_bar->to_delete = true;
		}
		to_delete = true;
	}

	checkAnimation(dt);
	//Debug features
	if (App->scene->debug && actualState != ST_BARRACK_PREVIEW)
	{
		App->render->DrawCircle(position.x, position.y, vision, 0, 0, 200);
		App->render->DrawCircle(position.x, position.y, collrange, 200, 200, 0);
		App->render->DrawCircle(position.x, position.y, body, 0, 0, 200);
		App->render->DrawQuad({ (int)position.x - 50, (int)position.y - 50, 100, 100 }, 200, 0, 0, 200, false);

		iPoint pos = { (int)position.x, (int)position.y };
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint tempPos = pos;

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				tempPos.x = pos.x + i;
				tempPos.y = pos.y + j;
				tempPos = App->map->MapToWorld(tempPos.x, tempPos.y);
				App->render->DrawQuad({ (int)(position.x + i * 32), (int)(position.y + j * 32), 32, 32 }, 200, 0, 0, 50);
			}
		}

	}

	//Final blit
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->building, world.x - 50, world.y - 50, r, 1.0f, 1.0f);

	//This render is placed behind the general blit for art purposes
	if (actualState == ST_BARRACK_PREVIEW) {

		if (canbuild)
		{

			App->render->DrawQuad({ world.x - 50, world.y - 50, 96, 95 }, 0, 200, 0, 100);
		}
		else
		{
			App->render->DrawQuad({ world.x - 50, world.y - 50, 96, 95 }, 200, 0, 0, 100);
		}
	}

	return true;
}

bool HumanBarracks::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool HumanBarracks::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	if (actualState != ST_BARRACK_PREVIEW)
	{
		iPoint pos = { (int)position.x, (int)position.y };
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint tempPos = pos;

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				tempPos.x = pos.x + i;
				tempPos.y = pos.y + j;
				App->pathfinding->ChangeWalkability(tempPos, 1);
			}
		}
	}
	else
	{
		App->scene->Building_preview = false;
	}
	return true;
}

void HumanBarracks::CheckWalkable(iPoint map)
{
	map.x -= 2;
	map.y -= 2;

	int tiles = 0;

	for (int i = 0; i < 3; i++)
	{
		map.y += 1;

		for (int d = 0; d < 3; d++)
		{
			map.x += 1;

			if (App->pathfinding->IsWalkable(map) == true)
			{
				tiles++;
			}
		}
		map.x -= 3;
	}
	if (tiles == 9)
	{
		canbuild = true;
	}
	else
	{
		canbuild = false;
	}
}

void HumanBarracks::checkAnimation(float dt)
{
	if (actualState == ST_BARRACK_AUTOMATIC) {
		App->scene->Building_preview = false;
		timer.Start();
		world.x = position.x;
		world.y = position.y;  
	
		iPoint pos = { (int)position.x, (int)position.y };
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint tempPos = pos;

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				tempPos.x = pos.x + i;
				tempPos.y = pos.y + j;
				App->pathfinding->ChangeWalkability(tempPos, 2);
			}
		}

		//SpatialAudio(1, App->audio->construction, position.x, position.y);

		actualState = ST_BARRACK_FINISHED;
	}

	if (actualState == ST_BARRACK_PREVIEW)
	{
		current_animation = &finishedconst;

		if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->input->screen_click)
		{

			//Mix_HaltChannel(-1);
			App->scene->Building_preview = false;
			timer.Start();
			GetTile();
			world.x += 32;
			world.y += 32;
			position.x = world.x;
			position.y = world.y;
			

			iPoint pos = { (int)position.x, (int)position.y };
			pos = App->map->WorldToMap(pos.x, pos.y);
			iPoint tempPos = pos;

			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					tempPos.x = pos.x + i;
					tempPos.y = pos.y + j;
					App->pathfinding->ChangeWalkability(tempPos, 2);
				}
			}

			SpatialAudio(1, App->audio->construction, position.x, position.y);

			actualState = ST_BARRANCK_IN_CONSTRUCTION;
			creation_barrack_bar = App->gui->CreateGuiElement(Types::bar, position.x-65, position.y-80, { 306, 107, 129, 9 }, nullptr, this, NULL);
		}
		
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && App->input->screen_click)
		{
		//	Mix_HaltChannel(-1);
			SpatialAudio(1, App->audio->cancel_building, position.x, position.y);
			App->scene->Building_preview = false;
			team = TeamType::PLAYER;
			to_delete = true;
		}

		GetTile();
		world.x += 32;
		world.y += 32;

		CheckWalkable(map);
	}

	if (actualState == ST_BARRANCK_IN_CONSTRUCTION)
	{
		float bar_prog = (timer.ReadSec() * 100) / 3;
		creation_barrack_bar->updateBar(bar_prog);
		current_animation = &inconstruction;
		team = TeamType::PLAYER;

		if (timer.ReadSec() >= construction_time)
		{
			//Mix_HaltChannel(-1);
			actualState = ST_BARRACK_FINISHED;
			if (creation_barrack_bar != nullptr) {
				creation_barrack_bar->to_delete = true;
			}
		}
		
	}

	if (actualState == ST_BARRACK_FINISHED)
	{
		//LOG("%d", Troop.size());
		// Finished Animation
		current_animation = &finishedconst;

		CheckQueue();

		if (timer_queue < 0)
		{
			timer_queue = 0;
		}
		if (timer_queue > 0) 
		{
			timer_queue = timer_queue - 1*dt;
		}
		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (createUI)
			{
				createUI = false;
				CreateBarrackUI();
				ImageSelected();
			}

			if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN && actualState != ST_BARRACK_UPGRADING && Barrack_Upgraded == false)
			{
				creation_barrack_bar = App->gui->CreateGuiElement(Types::bar, position.x - 65, position.y - 80, { 306, 107, 129, 9 }, nullptr, this, NULL);
				upgrade_timer.Start();
				actualState = ST_BARRACK_UPGRADING;
			}
			
			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN || create_swordman == true)
			{
				if (Troop.size() < 6)
				{
					timer_queue += 3;
					troop_type = 1;
					QueueTroop* item = new QueueTroop();
					item->time = timer_queue;
					item->type = troop_type;
		
					switch (Troop.size())
					{
					case 0:
						item->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 1:
						item->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 2:
						item->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 3:
						item->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 4:
						item->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 5:
						item->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					}
					Troop.push_back(item);
				}
				create_swordman = false;
			}
			if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && Barrack_Upgraded == true || create_archer == true)
			{
				if (Troop.size() < 6)
				{
					timer_queue += 3;
					troop_type = 2;
					QueueTroop* item = new QueueTroop();
					item->time = timer_queue;
					item->type = troop_type;
					switch (Troop.size())
					{
					case 0:
						item->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 1:
						item->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 2:
						item->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 3:
						item->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 4:
						item->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 5:
						item->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					}
					
					Troop.push_back(item);
				}
				create_archer = false;
			}
		}
		else
		{
			if (Button_Create_Footman != nullptr && createUI == false)
			{
				DeleteBarracksUI();
				createUI = true;
			}

			for (int i = 0; i < Troop.size(); i++)
			{
				if (Troop[i]->image != nullptr)
				{
					Troop[i]->image->to_delete = true;
				}
				if (Troop[i]->bar != nullptr)
				{
					Troop[i]->bar->to_delete = true;
				}
			}
		}

	}
	
	if (actualState == ST_BARRACK_UPGRADING)
	{
		float upgrade_bar = (upgrade_timer.ReadSec() * 100) / 10;
		creation_barrack_bar->updateBar(upgrade_bar);
		time_bar_start = false;
		if (Button_Create_Footman != nullptr) 
		{
			DeleteBarracksUI();
		}

		for (int i = 0; i < Troop.size(); i++)
		{
			if (Troop[i]->image != nullptr)
			{
				Troop[i]->image->to_delete = true;
			}
			if (Troop[i]->bar != nullptr)
			{
				Troop[i]->bar->to_delete = true;
			}
		}

		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);
		}
		//Timer for the upgrade
		if (upgrade_timer.ReadSec() >= first_upgrade_time )
		{
			if (creation_barrack_bar != nullptr)
			{
				creation_barrack_bar->to_delete = true;
			}
			Barrack_Upgraded = true;
			
			Troop.clear();
			timer_queue = 0;

			actualState = ST_BARRACK_FINISHED;
			createUI = true;
		}

	}

}

void HumanBarracks::CheckQueue()
{
	for (int i = 0; i < Troop.size(); i++)
	{
		if (Troop[i] == Troop[0] && Troop[0] != nullptr && time_bar_start == false)
		{
			timer_bar.Start();
			time_bar_start = true;
		}
			
		if (Troop[0] != nullptr)
		{
			float upgrade_bar = (timer_bar.ReadSec() * 100) / 3;
			Troop[0]->bar->updateBar(upgrade_bar);
		}

		if (Troop[i]->timer.ReadSec() >= Troop[i]->time)
		{
			QueueSwap();

			switch (Troop[i]->type)
			{
			case 1:
				App->requests->AddRequest(Petition::SPAWN, 0, SpawnTypes::SWORDMAN, { (int)position.x + 7, (int)position.y + 30 });
				if (Troop[i]->image != nullptr)
				{
					Troop[i]->image->to_delete = true;
					Troop[i]->image = nullptr;
				}
				if (Troop[i]->bar != nullptr)
				{
					Troop[i]->bar->to_delete = true;
				}
				time_bar_start = false;
				break;
			case 2:
				App->requests->AddRequest(Petition::SPAWN, 0, SpawnTypes::ARCHER, { (int)position.x + 7, (int)position.y + 30 });
				if (Troop[i]->image != nullptr)
				{
					Troop[i]->image->to_delete = true;
					Troop[i]->image = nullptr;
				}
				if (Troop[i]->bar != nullptr)
				{
					Troop[i]->bar->to_delete = true;
				}
				time_bar_start = false;
				break;
			}
			
			Troop.erase(Troop.begin() + i);
			i--;
		}
	}
}

void HumanBarracks::QueueSwap()
{
	if (Troop.size() == 6)
	{
		iPoint position0;
		Troop[4]->image->GetLocalPos(position0.x, position0.y);
		Troop[5]->image->SetLocalPos(position0.x, position0.y);
		Troop[4]->bar->GetLocalPos(position0.x, position0.y);
		Troop[5]->bar->SetLocalPos(position0.x, position0.y);
	}
	if (Troop.size() >= 5)
	{
		iPoint position0;
		Troop[3]->image->GetLocalPos(position0.x, position0.y);
		Troop[4]->image->SetLocalPos(position0.x, position0.y);
		Troop[3]->bar->GetLocalPos(position0.x, position0.y);
		Troop[4]->bar->SetLocalPos(position0.x, position0.y);
	}
	if (Troop.size() >= 4)
	{
		iPoint position0;
		Troop[2]->image->GetLocalPos(position0.x, position0.y);
		Troop[3]->image->SetLocalPos(position0.x, position0.y);
		Troop[2]->bar->GetLocalPos(position0.x, position0.y);
		Troop[3]->bar->SetLocalPos(position0.x, position0.y);
	}
	if (Troop.size() >= 3)
	{
		iPoint position0;
		Troop[1]->image->GetLocalPos(position0.x, position0.y);
		Troop[2]->image->SetLocalPos(position0.x, position0.y);
		Troop[1]->bar->GetLocalPos(position0.x, position0.y);
		Troop[2]->bar->SetLocalPos(position0.x, position0.y);
	}
	if (Troop.size() >= 2)
	{
		iPoint position0;
		Troop[0]->image->GetLocalPos(position0.x,position0.y);
		Troop[1]->image->SetLocalPos(position0.x, position0.y);
		Troop[0]->bar->GetLocalPos(position0.x, position0.y);
		Troop[1]->bar->SetLocalPos(position0.x, position0.y);
	}
}

void HumanBarracks::ImageSelected()
{
	if (Troop.size() == 6)
	{
		if (Troop[5]->type == 1)
		{
			Troop[5]->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		else if (Troop[5]->type == 2)
		{
			Troop[5]->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[5]->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 5)
	{
		if (Troop[4]->type == 1)
		{
			Troop[4]->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		else if (Troop[4]->type == 2)
		{
			Troop[4]->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[4]->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 4)
	{
		if (Troop[3]->type == 1)
		{
			Troop[3]->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		else if (Troop[3]->type == 2)
		{
			Troop[3]->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[3]->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 3)
	{
		if (Troop[2]->type == 1)
		{
			Troop[2]->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		else if (Troop[2]->type == 2)
		{
			Troop[2]->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[2]->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 2)
	{
		if (Troop[1]->type == 1)
		{
			Troop[1]->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		else if (Troop[1]->type == 2)
		{
			Troop[1]->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[1]->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 1)
	{
		if (Troop[0]->type == 1)
		{
			Troop[0]->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1186, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		else if (Troop[0]->type == 2)
		{
			Troop[0]->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1233, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[0]->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
}

void HumanBarracks::CreateBarrackUI()
{
	Button_Create_Footman = App->gui->CreateGuiElement(Types::button, 1000, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Footman->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Swordman_image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1186, 49, 46, 38 }, Button_Create_Footman, nullptr, NULL);
	
	if (Barrack_Upgraded == true) 
	{
		Button_Create_Archer = App->gui->CreateGuiElement(Types::button, 1100, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
		Button_Create_Archer->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
		Archer_image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1233, 49, 46, 38 }, Button_Create_Archer, nullptr, NULL);
	}
}

void HumanBarracks::DeleteBarracksUI()
{
	if (Button_Create_Footman != nullptr)
	{
		Button_Create_Footman->to_delete = true;
		Button_Create_Footman = nullptr;
	}
	
	if (Barrack_Upgraded && Button_Create_Archer != nullptr)
	{
		Button_Create_Archer->to_delete = true;
		Button_Create_Archer = nullptr;
	}
}


void HumanBarracks::GuiInput(GuiItem* guiElement) {
	if (guiElement == Button_Create_Footman) 
	{
		create_swordman = true;
		isSelected = true;
	}
	else if (guiElement == Button_Create_Archer) 
	{
		create_archer = true;
		isSelected = true;
	}
	
}