#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanTownHall.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"

HumanTownHall::HumanTownHall(int posx, int posy) : StaticEnt(StaticEntType::HumanTownHall)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 40;
	collrange = 25;
	selectable = false;
	isSelected = false;
	to_delete = false;
	canbuild = false;
	createUI = false;
	create_gatherer = false;
	selectable_buildings = true;
	time_bar_start = false;
	construction_time = 3;
	time_FX_barracks = 1;
	timer_queue = 0;
	pos0 = { 827, 103 };
	pos1 = { 890, 103 };
	pos2 = { 827, 168 };
	pos3 = { 890, 168 };
	pos4 = { 827, 230 };
	pos5 = { 890, 230 };
	// Load all animations
	inconstruction.PushBack({265,145,111,95}, 0.2, 0, 0, 0, 0);
	finishedconst2.PushBack({262,16,119,107}, 0.2, 0, 0, 0, 0);

	team = TeamType::NO_TYPE;
	actualState = ST_TOWNHALL_PREVIEW;
	life_points = 100;
}

HumanTownHall::~HumanTownHall()
{}

bool HumanTownHall::Start()
{
	if (App->scene->active == true) {
		actualState = ST_TOWNHALL_AUTOMATIC;
	}
	createUI = true;

	Button_Create_Gatherer = nullptr;
	Gatherer_image = nullptr;
	creation_TownHall_bar = nullptr;


	return true;
}

bool HumanTownHall::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		life_points = 0;
	}
	if (life_points <= 0) 
	{
		if (creation_TownHall_bar != nullptr) {
			creation_TownHall_bar->to_delete = true;
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
		if (Button_Create_Gatherer != nullptr)
		{
			DeleteTownHallUI();
		}
		to_delete = true;
	}
		
	checkAnimation(dt);
	//Debug features
	if (App->scene->debug && actualState != ST_TOWNHALL_PREVIEW)
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
	App->render->Blit(App->entity->building, world.x - 55, world.y - 55, r, 1.0f, 1.0f);

	//This render is placed behind the general blit for art purposes
	if (actualState == ST_TOWNHALL_PREVIEW) {

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

bool HumanTownHall::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool HumanTownHall::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	if (actualState != ST_TOWNHALL_PREVIEW)
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

void HumanTownHall::CheckWalkable(iPoint map)
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

void HumanTownHall::checkAnimation(float dt)
{
	if (actualState == ST_TOWNHALL_AUTOMATIC)
	{
		Mix_HaltChannel(-1);
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

		actualState = ST_TOWNHALL_FINISHED;
	}

	if (actualState == ST_TOWNHALL_PREVIEW)
	{
		current_animation = &finishedconst2;

		if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->input->screen_click)
		{

			Mix_HaltChannel(-1);
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

			creation_TownHall_bar = App->gui->CreateGuiElement(Types::bar, position.x - 65, position.y - 80, { 306, 107, 129, 9 }, nullptr, this, NULL);
			actualState = ST_TOWNHALL_IN_CONSTRUCTION;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && App->input->screen_click)
		{
			//Mix_HaltChannel(-1);
			SpatialAudio(1, App->audio->cancel_building, position.x, position.y);
			team = TeamType::PLAYER;
			App->scene->Building_preview = false;
			to_delete = true;
		}

		GetTile();
		world.x += 32;
		world.y += 32;

		CheckWalkable(map);
	}

	if (actualState == ST_TOWNHALL_IN_CONSTRUCTION)
	{
		float bar_prog = (timer.ReadSec() * 100) / 3;
		creation_TownHall_bar->updateBar(bar_prog);
		current_animation = &inconstruction;
		team = TeamType::PLAYER;

		if (timer.ReadSec() >= construction_time)
		{
			if (creation_TownHall_bar != nullptr) {
				creation_TownHall_bar->to_delete = true;
			}
			Mix_HaltChannel(-1);
			actualState = ST_TOWNHALL_FINISHED;
			Mix_HaltChannel(-1);
		}
		/*else {
			if (timer.ReadSec() >= time_FX_barracks) {
				SpatialAudio(1, App->audio->construction, position.x, position.y);
				time_FX_barracks++;
			}
		}*/

	}

	if (actualState == ST_TOWNHALL_FINISHED)
	{
		// Finished Animation
		current_animation = &finishedconst2;

		CheckQueue();

		if (timer_queue < 0)
		{
			timer_queue = 0;
		}
		if (timer_queue > 0)
		{
			timer_queue = timer_queue - 1 * dt;
		}
		if (isSelected == true)
		{
			if (createUI)
			{
				createUI = false;
				CreateTownHallUI();
				ImageSelected();
			}

			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN || create_gatherer == true)
			{
				if (Troop.size() < 6)
				{
					timer_queue += 3;
					QueueTroop* item = new QueueTroop();
					item->time = timer_queue;

					switch (Troop.size())
					{
					case 0:
						item->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 1:
						item->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 2:
						item->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 3:
						item->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 4:
						item->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 5:
						item->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					}
					Troop.push_back(item);
				}
				create_gatherer = false;
			}
		}
		else
		{
			if (Button_Create_Gatherer != nullptr && createUI == false)
			{
				DeleteTownHallUI();
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
}

void HumanTownHall::CheckQueue()
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

			App->requests->AddRequest(Petition::SPAWN, 0, SpawnTypes::GATHERER, { (int)position.x + 7, (int)position.y + 30 });
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

			Troop.erase(Troop.begin() + i);
			i--;
		}
	}
}

void HumanTownHall::QueueSwap()
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
		Troop[0]->image->GetLocalPos(position0.x, position0.y);
		Troop[1]->image->SetLocalPos(position0.x, position0.y);
		Troop[0]->bar->GetLocalPos(position0.x, position0.y);
		Troop[1]->bar->SetLocalPos(position0.x, position0.y);
	}
}

void HumanTownHall::ImageSelected()
{
	if (Troop.size() == 6)
	{
		Troop[5]->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[5]->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 5)
	{
		Troop[4]->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[4]->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 4)
	{
		Troop[3]->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[3]->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 3)
	{
		Troop[2]->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[2]->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 2)
	{
		Troop[1]->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[1]->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 1)
	{
		Troop[0]->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[0]->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 20, { 306, 107, 129, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
}

void HumanTownHall::CreateTownHallUI()
{
	Button_Create_Gatherer = App->gui->CreateGuiElement(Types::button, 1000, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Gatherer->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Gatherer_image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1140, 49, 46, 38 }, Button_Create_Gatherer, nullptr, NULL);
}

void HumanTownHall::DeleteTownHallUI()
{
	if (Button_Create_Gatherer != nullptr) 
	{
		Button_Create_Gatherer->to_delete = true;
		Button_Create_Gatherer = nullptr;
	}
}


void HumanTownHall::GuiInput(GuiItem* guiElement) {
	if (guiElement == Button_Create_Gatherer) {
		create_gatherer = true;
		isSelected = true;
	}
}