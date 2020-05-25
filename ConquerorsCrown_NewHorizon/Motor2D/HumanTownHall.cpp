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
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Tutorial.h"

HumanTownHall::HumanTownHall(int posx, int posy) : StaticEnt(StaticEntType::HumanTownHall)
{
	name.create("town_hall");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 40;
	coll_range = 50;
	selectable = false;
	isSelected = false;
	to_delete = false;
	canbuild = false;
	active = true;
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

	team = TeamType::PLAYER;
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
	Gatherer_stone_cost = nullptr;
	Gatherer_gold_cost = nullptr;
	creation_TownHall_bar = nullptr;
	Gatherer_Text_stone = nullptr;
	Gatherer_Text_Gold = nullptr;
	Button_Create_Lab;
	Lab_Image = nullptr;
	Lab_stone_cost = nullptr;
	Lab_Text_stone = nullptr;
	Lab_wood_cost = nullptr;
	Lab_Text_Wood = nullptr;
	Button_Create_Wall = nullptr;
	Wall_Image = nullptr;
	Wall_stone_cost = nullptr;
	Wall_Text_stone = nullptr;

	deployed = false;
	return true;
}

bool HumanTownHall::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (isSelected && App->movement->player_selected != this)
		isSelected = false;
	if(App->scene->debug)
	life_points = 100;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;

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
		//App->render->DrawCircle(position.x, position.y, vision, 0, 0, 200);
		//App->render->DrawCircle(position.x, position.y, collrange, 200, 200, 0);
		//App->render->DrawCircle(position.x, position.y, body, 0, 0, 200);
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
		deployed = true;
		map = App->map->WorldToMap(position.x, position.y);
		Mix_HaltChannel(-1);
		App->scene->Building_preview = false;
		timer.Start();
		team = TeamType::PLAYER;
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

		if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->input->screen_click && App->scene->wood >= 200 && App->scene->stone >= 300 || App->scene->debug == true && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true)
		{
			if (App->scene->debug == false)
			{
				App->scene->AddResource("wood", -200);
				App->scene->AddResource("stone", -300);
			}
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
		deployed = true;
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
		
		if (isSelected == true && App->tutorial->ActualState != ST_Tutorial_Q4)
		{
			
			if (createUI)
			{
				createUI = false;
				CreateTownHallUI();
				ImageSelected();

				// Tutorial
				if (App->tutorial->ActualState == ST_Tutorial_Q5)
				{
					App->tutorial->Arrow_4->to_delete = true;
					App->tutorial->Arrow_4 = nullptr;
					App->tutorial->Arrow_5 = App->gui->CreateGuiElement(Types::image, 1005, 450, { 2656, 212, 45, 64 }, App->scene->ingameTopBar);
				}
			}

			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && App->scene->wood >= 100 && App->scene->gold >= 100 || create_gatherer == true && App->scene->wood >= 100 && App->scene->gold >= 100 ||
				App->scene->debug == true && App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN || App->scene->debug == true && create_gatherer == true)
			{
				if (App->scene->debug == false) 
				{
					App->scene->AddResource("wood", -100);
					App->scene->AddResource("gold", -100);
				}

				if (Troop.size() < 6)
				{
					timer_queue += 3;
					QueueTroop* item = new QueueTroop();
					item->time = timer_queue;

					switch (Troop.size())
					{
					case 0:
						item->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 1:
						item->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 2:
						item->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 3:
						item->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 4:
						item->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 5:
						item->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					}
					Troop.push_back(item);
				}
				create_gatherer = false;
			}
		}
		else
		{
			if (Button_Create_Gatherer != nullptr && createUI == false && Gatherer_gold_cost != nullptr )
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
			Searchtile(map);
			int randomrespawn = rand() % 10 + 10;
			App->requests->AddRequest(Petition::SPAWN, 0, SpawnTypes::GATHERER, { respawn.x +randomrespawn, respawn.y+randomrespawn });
			
			// Tutorial
			if (App->scene->current_scene == scenes::tutorial && App->tutorial->ActualState == ST_Tutorial_Q5)
			{
				App->tutorial->Arrow_5_1->to_delete = true;
				App->tutorial->Arrow_5_1 = nullptr;
				App->audio->PlayFx(-1, App->audio->quest_complete, 0);
				App->tutorial->ActualState = ST_Tutorial_Q6;
				App->tutorial->deleteUI(0);
			}
			
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
		Troop[5]->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 5)
	{
		Troop[4]->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[4]->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 4)
	{
		Troop[3]->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[3]->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 3)
	{
		Troop[2]->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[2]->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 2)
	{
		Troop[1]->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[1]->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 1)
	{
		Troop[0]->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 1140, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		Troop[0]->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
}

void HumanTownHall::CreateTownHallUI()
{
	Button_Create_Lab = App->gui->CreateGuiElement(Types::button, 1195, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Lab->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Lab_Image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1045, 49, 46, 38 }, Button_Create_Lab, nullptr, NULL);
	Lab_stone_cost = App->gui->CreateGuiElement(Types::image, 1185, 140, { 974, 5, 75, 26 }, App->scene->ingameUI, nullptr, NULL);
	Lab_Text_stone = App->gui->CreateGuiElement(Types::text, 1215, 140, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "900", App->font->smallfont);
	Lab_wood_cost = App->gui->CreateGuiElement(Types::image, 1185, 165, { 832, 5, 75, 26 }, App->scene->ingameUI, nullptr, NULL);
	Lab_Text_Wood = App->gui->CreateGuiElement(Types::text, 1215, 165, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "900", App->font->smallfont);

	Button_Create_Barrack = App->gui->CreateGuiElement(Types::button, 1095, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Barrack->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	BarrackImage = App->gui->CreateGuiElement(Types::image, 6, 6, { 1045, 49, 46, 38 }, Button_Create_Barrack, nullptr, NULL);
	Barrack_wood_cost = App->gui->CreateGuiElement(Types::image, 1085, 165, { 832, 5, 75, 26 }, App->scene->ingameUI, nullptr, NULL);
	Barrack_Text_Wood = App->gui->CreateGuiElement(Types::text, 1115, 165, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "100", App->font->smallfont);
	Barrack_stone_cost = App->gui->CreateGuiElement(Types::image, 1085, 140, { 974, 5, 75, 26 }, App->scene->ingameUI, nullptr, NULL);
	Barrack_Text_stone = App->gui->CreateGuiElement(Types::text, 1115, 140, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "200", App->font->smallfont);

	Button_Create_Wall = App->gui->CreateGuiElement(Types::button, 1000, 200, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Wall->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Wall_Image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1140, 49, 46, 38 }, Button_Create_Wall, nullptr, NULL);
	Wall_stone_cost = App->gui->CreateGuiElement(Types::image, 1070, 215, { 974, 5, 75, 26 }, App->scene->ingameUI, nullptr, NULL);
	Wall_Text_stone = App->gui->CreateGuiElement(Types::text, 1100, 215, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "100", App->font->smallfont);

	Button_Create_Gatherer = App->gui->CreateGuiElement(Types::button, 1000, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Gatherer->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Gatherer_image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1140, 49, 46, 38 }, Button_Create_Gatherer, nullptr, NULL);
	Gatherer_gold_cost = App->gui->CreateGuiElement(Types::image, 990, 140, { 690, 5, 75, 26 }, App->scene->ingameUI, nullptr, NULL);
	Gatherer_Text_Gold = App->gui->CreateGuiElement(Types::text, 1020, 140, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "100", App->font->smallfont);
	Gatherer_stone_cost = App->gui->CreateGuiElement(Types::image, 990, 165, { 832, 5, 75, 26 }, App->scene->ingameUI, nullptr, NULL);
	Gatherer_Text_stone = App->gui->CreateGuiElement(Types::text, 1020, 165, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "100", App->font->smallfont);
}

void HumanTownHall::DeleteTownHallUI()
{
	if (Button_Create_Barrack != nullptr)
	{
		Button_Create_Barrack->to_delete = true;
		Barrack_stone_cost->to_delete = true;
		Barrack_Text_stone->to_delete = true;
		Barrack_wood_cost->to_delete = true;
		Barrack_Text_Wood->to_delete = true;

		Button_Create_Barrack = nullptr;
		Barrack_stone_cost = nullptr;
		Barrack_Text_stone = nullptr;
		Barrack_wood_cost = nullptr;
		Barrack_Text_Wood = nullptr;
	}

	if (Button_Create_Gatherer != nullptr) 
	{
		Gatherer_stone_cost->to_delete = true;
		Gatherer_Text_stone->to_delete = true;
		Gatherer_Text_Gold->to_delete = true;
		Gatherer_gold_cost->to_delete = true;
		Button_Create_Gatherer->to_delete = true;

		Gatherer_stone_cost = nullptr;
		Gatherer_Text_stone = nullptr;
		Gatherer_Text_Gold = nullptr;
		Gatherer_gold_cost = nullptr;
		Button_Create_Gatherer = nullptr;
	}
	if (Button_Create_Lab != nullptr)
	{
		Button_Create_Lab->to_delete = true;
		Lab_Image->to_delete = true;
		Lab_stone_cost->to_delete = true;
		Lab_Text_stone->to_delete = true;
		Lab_wood_cost->to_delete = true;
		Lab_Text_Wood->to_delete = true;

		Button_Create_Lab = nullptr;
		Lab_Image = nullptr;
		Lab_stone_cost = nullptr;
		Lab_Text_stone = nullptr;
		Lab_wood_cost = nullptr;
		Lab_Text_Wood = nullptr;
	}
	if (Button_Create_Wall != nullptr)
	{
		Button_Create_Wall->to_delete = true;
		Wall_Image->to_delete = true;
		Wall_stone_cost->to_delete = true;
		Wall_Text_stone->to_delete = true;

		Button_Create_Wall = nullptr;
		Wall_Image = nullptr;
		Wall_stone_cost = nullptr;
		Wall_Text_stone = nullptr;
	}
}


void HumanTownHall::GuiInput(GuiItem* guiElement) {
	if (guiElement == Button_Create_Gatherer) {
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		if (App->scene->wood >= 100 && App->scene->gold >= 100 || App->scene->debug == true) {
			create_gatherer = true;

			// Tutorial
			if (App->tutorial->ActualState == ST_Tutorial_Q5)
			{
				App->tutorial->Arrow_5->to_delete = true;
				App->tutorial->Arrow_5 = nullptr;
				App->tutorial->Arrow_5_1 = App->gui->CreateGuiElement(Types::image, 830, 455, { 2656, 212, 45, 64 }, App->scene->ingameTopBar);
			}
		}
		isSelected = true;
	}

	if (guiElement == Button_Create_Barrack) {
		if (App->scene->wood >= 100 && App->scene->stone >= 100 && App->scene->Building_preview == false || App->scene->debug == true && App->scene->Building_preview == false)
		{
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanBarracks,App->scene->mouse_position.x, App->scene->mouse_position.y);
			App->scene->Building_preview = true;
		}
	}

	if (guiElement == Button_Create_Lab) {
		if (App->scene->wood >= 900 && App->scene->stone >= 900 && App->scene->Building_preview == false || App->scene->debug == true && App->scene->Building_preview == false)
		{
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanUpgrade, App->scene->mouse_position.x, App->scene->mouse_position.y);
			App->scene->Building_preview = true;
		}
	}

	if (guiElement == Button_Create_Wall) {
		if (App->scene->stone >= 100 && App->scene->Building_preview == false || App->scene->debug == true && App->scene->Building_preview == false)
		{
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanWall, App->scene->mouse_position.x, App->scene->mouse_position.y);
			App->scene->wall_create = true;
			App->scene->Building_preview = true;
		}
	}
}

iPoint HumanTownHall::Searchtile(iPoint map) 
{
	map.x += 3;
	map.y += 3;
	iPoint home =map;
	iPoint normal_start = map;
	int loops=0;

	normal_start.y -= 1;
	normal_start.x -= 3;

	if (App->pathfinding->IsWalkable(normal_start) == true) 
	{
		respawn = normal_start;
		respawn = App->map->MapToWorld(respawn.x, respawn.y);
		return respawn;
	}

	do
	{
		if (loops >= 1) 
		{
			home.x += 1; 
			home.y += 1;
			map = home;

		}
		for (int i = 0; i < 5 + loops; i++)
		{
			map.y -= 1;

			for (int d = 0; d < 5 + loops; d++)
			{
				map.x -= 1;

				if (App->pathfinding->IsWalkable(map) == true)
				{
					respawn = { map.x,map.y };
					respawn = App->map->MapToWorld(respawn.x, respawn.y);
					return respawn;
				}
			}
			map.x += 5 + loops;
		}

		loops++;
	} while (loops < 10);

	respawn = { 1,1 };
	return respawn;
}