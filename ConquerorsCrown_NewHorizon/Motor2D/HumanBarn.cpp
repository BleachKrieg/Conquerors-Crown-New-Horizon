#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanBarn.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Audio.h"
#include "j1Tutorial.h"
#include "FoWManager.h"

HumanBarn::HumanBarn(int posx, int posy) : StaticEnt(StaticEntType::Barn)
{
	name.create("human_barn");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 40;
	coll_range = 70;
	active = true;
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
	inconstruction.PushBack({ 279,403,93,96 }, 0.2, 0, 0, 0, 0);
	finishedconst.PushBack({ 276,273,95,98 }, 0.2, 0, 0, 0, 0);
	team = TeamType::PLAYER;
	actualState = ST_BARN_PREVIEW;
	life_points = 100;
	max_hp = life_points;
	createUI = false;
	visionEntity = nullptr;
}

HumanBarn::~HumanBarn()
{}

bool HumanBarn::Start()
{
	if (App->scene->active == true) {
		actualState = ST_BARN_AUTOMATIC;
	}
	Button_Create_Footman = nullptr;
	createUI = true;
	Swordman_image = nullptr;
	Swordman_stone_cost = nullptr;
	Swordman_Text_stone = nullptr;
	Swordman_gold_cost = nullptr;
	Swordman_Text_Gold = nullptr;
	creation_barrack_bar = nullptr;
	visionEntity = nullptr;
	deployed = false;

	return true;
}

bool HumanBarn::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (isSelected && App->movement->player_selected != this)
		isSelected = false;

	if (App->scene->debug)
		life_points = 100;

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;

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
	if (App->scene->debug && actualState != ST_BARN_PREVIEW)
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
	App->render->Blit(App->entity->building, world.x - 50, world.y - 50, r, 1.0f, 1.0f);

	hp_conversion = (float)40 / (float)max_hp;
	SDL_Rect section;
	section.x = 0;
	section.y = 0;
	section.w = ((int)life_points * hp_conversion);
	section.h = 2;
	App->render->Blit(App->entity->life_bar, (int)(position.x - (*r).w / 4), (int)(position.y + (*r).h / 3), &section);

	//This render is placed behind the general blit for art purposes
	if (actualState == ST_BARN_PREVIEW) {

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

bool HumanBarn::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool HumanBarn::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	if (actualState != ST_BARN_PREVIEW)
	{
		iPoint pos = { (int)position.x, (int)position.y };
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint tempPos = pos;
		visionEntity->deleteEntity = true;
		App->fowManager->foWMapNeedsRefresh = true;

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

void HumanBarn::CheckWalkable(iPoint map)
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

void HumanBarn::checkAnimation(float dt)
{
	if (actualState == ST_BARN_AUTOMATIC) {
		map = App->map->WorldToMap(position.x, position.y);
		App->scene->Building_preview = false;
		timer.Start();
		world.x = position.x;
		world.y = position.y;
		team = TeamType::PLAYER;

		// Fog of war
		if (visionEntity == nullptr)
		{
			iPoint pos = { (int)position.x, (int)position.y };
			visionEntity = App->fowManager->CreateFoWEntity({ pos.x, pos.y }, true);
			visionEntity->SetNewVisionRadius(5);
		}

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

		actualState = ST_BARN_FINISHED;
	}

	if (actualState == ST_BARN_PREVIEW)
	{
		current_animation = &finishedconst;

		if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->input->screen_click && App->scene->wood >= 400 && App->scene->stone >= 200 || (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->scene->debug == true)
		{
			if (App->scene->debug == false)
			{
				App->scene->AddResource("wood", -400);
				App->scene->AddResource("stone", -200);
			}
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

			actualState = ST_BARN_IN_CONSTRUCTION;
			creation_barrack_bar = App->gui->CreateGuiElement(Types::bar, position.x - 65, position.y - 80, { 306, 107, 129, 9 }, nullptr, this, NULL);
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && App->input->screen_click)
		{
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

	if (actualState == ST_BARN_IN_CONSTRUCTION)
	{
		deployed = true;
		float bar_prog = (timer.ReadSec() * 100) / 3;
		creation_barrack_bar->updateBar(bar_prog);
		current_animation = &inconstruction;
		team = TeamType::PLAYER;

		// Fog of war
		if (visionEntity == nullptr)
		{
			iPoint pos = { (int)position.x, (int)position.y };
			visionEntity = App->fowManager->CreateFoWEntity({ pos.x, pos.y }, true);
			visionEntity->SetNewVisionRadius(5);
		}

		if (timer.ReadSec() >= construction_time)
		{
			//Mix_HaltChannel(-1);
			actualState = ST_BARN_FINISHED;
			if (creation_barrack_bar != nullptr) {
				creation_barrack_bar->to_delete = true;
			}
		}

	}

	if (actualState == ST_BARN_FINISHED)
	{
		//LOG("%d", Troop.size());
		// Finished Animation
		current_animation = &finishedconst;

		if (App->scene->current_scene == scenes::tutorial && App->tutorial->ActualState == ST_Tutorial_Q9_1)
		{
			if (App->tutorial->mision1 != nullptr && App->tutorial->mision1_Text != nullptr && App->tutorial->mision1_Text_2 != nullptr)
			{
				App->tutorial->mision1->to_delete = true;
				App->tutorial->mision1_Text->to_delete = true;
				App->tutorial->mision1_Text_2->to_delete = true;

				App->tutorial->mision1 = nullptr;
				App->tutorial->mision1_Text = nullptr;
				App->tutorial->mision1_Text_2 = nullptr;
			}
			App->audio->PlayFx(-1, App->audio->quest_complete, 0);
			App->tutorial->ActualState = ST_Tutorial_Q10;
		}

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
			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (createUI)
			{
				createUI = false;
				CreateBarrackUI();
				ImageSelected();
			}

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && App->scene->wood >= 250 && App->scene->gold >= 250 || create_swordman == true && App->scene->wood >= 250 && App->scene->gold >= 250 || App->scene->debug == true && create_swordman == true || App->scene->debug == true && App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				if (App->scene->debug == false)
				{
					App->scene->AddResource("wood", -250);
					App->scene->AddResource("gold", -250);
				}

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
						item->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 1:
						item->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 2:
						item->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 3:
						item->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 4:
						item->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					case 5:
						item->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
						item->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
						break;
					}
					Troop.push_back(item);
				}
				create_swordman = false;
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
}

void HumanBarn::CheckQueue()
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
				Searchtile(map);
				randomrespawn = rand() % 10 + 10;
				App->requests->AddRequest(Petition::SPAWN, 0, SpawnTypes::KNIGHT, { respawn.x + randomrespawn, respawn.y + randomrespawn });

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

void HumanBarn::QueueSwap()
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

void HumanBarn::ImageSelected()
{
	if (Troop.size() == 6)
	{
		if (Troop[5]->type == 1)
		{
			Troop[5]->image = App->gui->CreateGuiElement(Types::image, pos5.x, pos5.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[5]->bar = App->gui->CreateGuiElement(Types::bar, pos5.x, pos5.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 5)
	{
		if (Troop[4]->type == 1)
		{
			Troop[4]->image = App->gui->CreateGuiElement(Types::image, pos4.x, pos4.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[4]->bar = App->gui->CreateGuiElement(Types::bar, pos4.x, pos4.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 4)
	{
		if (Troop[3]->type == 1)
		{
			Troop[3]->image = App->gui->CreateGuiElement(Types::image, pos3.x, pos3.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[3]->bar = App->gui->CreateGuiElement(Types::bar, pos3.x, pos3.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 3)
	{
		if (Troop[2]->type == 1)
		{
			Troop[2]->image = App->gui->CreateGuiElement(Types::image, pos2.x, pos2.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[2]->bar = App->gui->CreateGuiElement(Types::bar, pos2.x, pos2.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 2)
	{
		if (Troop[1]->type == 1)
		{
			Troop[1]->image = App->gui->CreateGuiElement(Types::image, pos1.x, pos1.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[1]->bar = App->gui->CreateGuiElement(Types::bar, pos1.x, pos1.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
	if (Troop.size() >= 1)
	{
		if (Troop[0]->type == 1)
		{
			Troop[0]->image = App->gui->CreateGuiElement(Types::image, pos0.x, pos0.y, { 998, 49, 46, 38 }, App->scene->ingameUI, nullptr, NULL);
		}
		Troop[0]->bar = App->gui->CreateGuiElement(Types::bar, pos0.x, pos0.y + 37, { 436, 107, 46, 9 }, App->scene->ingameUI, nullptr, NULL);
	}
}

void HumanBarn::CreateBarrackUI()
{
	Button_Create_Footman = App->gui->CreateGuiElement(Types::button, 1000, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Footman->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Swordman_image = App->gui->CreateGuiElement(Types::image, 6, 6, { 998, 49, 46, 38 }, Button_Create_Footman, nullptr, NULL);
	Swordman_gold_cost = App->gui->CreateGuiElement(Types::image, 990, 140, { 690, 5, 85, 26 }, App->scene->ingameUI, nullptr, NULL);
	Swordman_Text_Gold = App->gui->CreateGuiElement(Types::text, 1020, 140, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "250", App->font->smallfont);
	Swordman_stone_cost = App->gui->CreateGuiElement(Types::image, 990, 165, { 832, 5, 85, 26 }, App->scene->ingameUI, nullptr, NULL);
	Swordman_Text_stone = App->gui->CreateGuiElement(Types::text, 1020, 165, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "250", App->font->smallfont);
}

void HumanBarn::DeleteBarracksUI()
{
	if (Button_Create_Footman != nullptr)
	{
		Button_Create_Footman->to_delete = true;
		Swordman_gold_cost->to_delete = true;
		Swordman_Text_Gold->to_delete = true;
		Swordman_stone_cost->to_delete = true;
		Swordman_Text_stone->to_delete = true;

		Swordman_gold_cost = nullptr;
		Swordman_Text_Gold = nullptr;
		Swordman_stone_cost = nullptr;
		Swordman_Text_stone = nullptr;
		Button_Create_Footman = nullptr;
	}
}


void HumanBarn::GuiInput(GuiItem* guiElement) {
	if (guiElement == Button_Create_Footman)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		if (App->scene->wood >= 50 && App->scene->gold >= 100 || App->scene->debug == true) {
			create_swordman = true;
		}
		isSelected = true;
	}
}


iPoint HumanBarn::Searchtile(iPoint map)
{
	map.x += 3;
	map.y += 3;
	iPoint home = map;
	iPoint normal_start = map;
	int loops = 0;

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