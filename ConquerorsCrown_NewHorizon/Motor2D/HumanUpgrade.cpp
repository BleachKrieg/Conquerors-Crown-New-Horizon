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
#include "j1Fonts.h"
#include "j1Audio.h"
#include "j1Tutorial.h"
#include "HumanUpgrade.h"

Human_Upgrade::Human_Upgrade(int posx, int posy) : StaticEnt(StaticEntType::HumanUpgrade)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 40;
	active = true;
	selectable = true;
	isSelected = false;
	to_delete = false;
	canbuild = false;
	first_upgrade_time = 10;
	//selectable_buildings = true;
	construction_time = 3;
	time_FX_barracks = 1;
	// Load all animations
	inconstruction.PushBack({ 278,659,82,92 }, 0.2, 0, 0, 0, 0);
	finishedconst.PushBack({ 273,532,95,94 }, 0.2, 0, 0, 0, 0);
	team = TeamType::PLAYER;
	actualState = ST_UPGRADE_PREVIEW;
	life_points = 100;
	createUI = false;
}

Human_Upgrade::~Human_Upgrade()
{}

bool Human_Upgrade::Start()
{
	if (App->scene->active == true) {
		actualState = ST_UPGRADE_AUTOMATIC;
	}
	barrack_Ui_upgrade = false;
	createUI = true;
	Button_Upgrade = nullptr;
	Barrack_image = nullptr;
	Arrow_image = nullptr;
	Upgrade_wood_cost = nullptr;
	Upgrade_Text_wood = nullptr;
	Button_Upgrade_Footman = nullptr;
	Swordman_image = nullptr;
	Swordman_stone_cost = nullptr;
	Swordman_Text_stone = nullptr;
	Swordman_gold_cost = nullptr;
	Swordman_Text_Gold = nullptr;
	Button_Create_Archer = nullptr;
	Archer_image = nullptr;
	Archer_stone_cost = nullptr;
	Archer_Text_stone = nullptr;
	Archer_gold_cost = nullptr;
	Archer_Text_Gold = nullptr;
	creation_barrack_bar = nullptr;
	deployed = false;
	Upgrading_swordman = false;
	return true;
}

bool Human_Upgrade::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (isSelected && App->movement->player_selected != this)
		isSelected = false;

	if (App->scene->debug)
		life_points = 100;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;

	if (life_points <= 0)
	{
		if (Button_Upgrade_Footman != nullptr)
		{
			DeleteUpgradeUI();
		}
		if (creation_barrack_bar != nullptr)
		{
			creation_barrack_bar->to_delete = true;
		}
		to_delete = true;
	}

	checkAnimation(dt);
	//Debug features
	if (App->scene->debug && actualState != ST_UPGRADE_PREVIEW)
	{

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
	if (actualState == ST_UPGRADE_PREVIEW) {

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

bool Human_Upgrade::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool Human_Upgrade::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	if (actualState != ST_UPGRADE_PREVIEW)
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

void Human_Upgrade::CheckWalkable(iPoint map)
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

void Human_Upgrade::checkAnimation(float dt)
{
	if (actualState == ST_UPGRADE_AUTOMATIC) {
		map = App->map->WorldToMap(position.x, position.y);
		App->scene->Building_preview = false;
		timer.Start();
		world.x = position.x;
		world.y = position.y;
		team = TeamType::PLAYER;

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

		actualState = ST_UPGRADE_FINISHED;
	}

	if (actualState == ST_UPGRADE_PREVIEW)
	{
		current_animation = &finishedconst;

		if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->input->screen_click/* && App->scene->wood >= 100 && App->scene->stone >= 200 */|| (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->scene->debug == true)
		{
			if (App->scene->debug == false)
			{
				App->scene->AddResource("wood", -900);
				App->scene->AddResource("stone", -900);
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

			actualState = ST_UPGRADE_IN_CONSTRUCTION;
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

	if (actualState == ST_UPGRADE_IN_CONSTRUCTION)
	{
		deployed = true;
		float bar_prog = (timer.ReadSec() * 100) / 3;
		creation_barrack_bar->updateBar(bar_prog);
		current_animation = &inconstruction;
		team = TeamType::PLAYER;

		if (timer.ReadSec() >= construction_time)
		{
			//Mix_HaltChannel(-1);
			actualState = ST_UPGRADE_FINISHED;
			if (creation_barrack_bar != nullptr) {
				creation_barrack_bar->to_delete = true;
			}
		}

	}

	if (actualState == ST_UPGRADE_FINISHED)
	{
		//LOG("%d", Troop.size());
		// Finished Animation
		current_animation = &finishedconst;

		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (createUI)
			{
				createUI = false;
				CreateUpgradeUI();
			}

			if (Swordman_Upgrade == true && App->scene->wood >= 999 && App->scene->gold >= 999 ||App->scene->debug == true && Swordman_Upgrade == true)
			{
				Swordman_Upgrade = false;
				if (App->scene->debug == false)
				{
					App->scene->AddResource("wood", -999);
					App->scene->AddResource("gold", -999);
				}
				upgrade_timer2.Start();
				creation_Upgrade_bar = App->gui->CreateGuiElement(Types::bar, position.x - 65, position.y - 80, { 306, 107, 129, 9 }, nullptr, this, NULL);
				actualState = ST_UPGRADE_UPGRADING;
			}
			if (Archer_Upgrade == true && App->scene->wood >= 999 && App->scene->gold >= 999 ||App->scene->debug == true && Archer_Upgrade == true)
			{
				Archer_Upgrade = false;
				if (App->scene->debug == false)
				{
					App->scene->AddResource("wood", -999);
					App->scene->AddResource("gold", -999);
				}
				upgrade_timer2.Start();
				creation_Upgrade_bar_archer = App->gui->CreateGuiElement(Types::bar, position.x - 65, position.y - 80, { 306, 107, 129, 9 }, nullptr, this, NULL);
				actualState = ST_UPGRADE_UPGRADING;

			}
		}
		else
		{
			if (Button_Upgrade_Footman != nullptr && createUI == false)
			{
				DeleteUpgradeUI();
				createUI = true;
			}
		}
	}

	if (actualState == ST_UPGRADE_UPGRADING)
	{
	
		if (Upgrading_swordman == true)
		{
			float upgrade_bar = (upgrade_timer2.ReadSec() * 100) / 10;
			creation_Upgrade_bar->updateBar(upgrade_bar);
			time_bar_start = false;
			if (Button_Upgrade_Footman != nullptr)
			{
				DeleteUpgradeUI();
			}
			if (isSelected == true)
			{
				App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);
			}
			//Timer for the upgrade
			if (upgrade_timer2.ReadSec() >= first_upgrade_time)
			{
				App->audio->PlayFx(1, App->audio->upgrade_complete, 0);
				App->scene->Upgrade_Sowrdman = true;
				Upgrading_swordman = false;
				if (creation_Upgrade_bar != nullptr)
				{
					creation_Upgrade_bar->to_delete = true;
				}
				actualState = ST_UPGRADE_FINISHED;
				createUI = true;
			}
		}

		if (Upgrading_Archer == true)
		{
			float upgrade_bar = (upgrade_timer2.ReadSec() * 100) / 10;
			creation_Upgrade_bar_archer->updateBar(upgrade_bar);
			time_bar_start = false;
			if (Button_Upgrade_Footman != nullptr)
			{
				DeleteUpgradeUI();
			}
			if (isSelected == true)
			{
				App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);
			}
			//Timer for the upgrade
			if (upgrade_timer2.ReadSec() >= first_upgrade_time)
			{
				App->audio->PlayFx(1, App->audio->upgrade_complete, 0);
				App->scene->Upgrade_Archer = true;
				Upgrading_Archer = false;
				if (creation_Upgrade_bar_archer != nullptr)
				{
					creation_Upgrade_bar_archer->to_delete = true;
				}
				actualState = ST_UPGRADE_FINISHED;
				createUI = true;
			}
		}
	}
}

void Human_Upgrade::CreateUpgradeUI()
{


	Button_Upgrade_Footman = App->gui->CreateGuiElement(Types::button, 1000, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Upgrade_Footman->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Swordman_image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1186, 49, 46, 38 }, Button_Upgrade_Footman, nullptr, NULL);
	Swordman_gold_cost = App->gui->CreateGuiElement(Types::image, 990, 140, { 690, 5, 85, 26 }, App->scene->ingameUI, nullptr, NULL);
	Swordman_Text_Gold = App->gui->CreateGuiElement(Types::text, 1020, 140, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "999", App->font->smallfont);
	Swordman_stone_cost = App->gui->CreateGuiElement(Types::image, 990, 165, { 832, 5, 85, 26 }, App->scene->ingameUI, nullptr, NULL);
	Swordman_Text_stone = App->gui->CreateGuiElement(Types::text, 1020, 165, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "999", App->font->smallfont);
	
	Button_Create_Archer = App->gui->CreateGuiElement(Types::button, 1100, 80, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
	Button_Create_Archer->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	Archer_image = App->gui->CreateGuiElement(Types::image, 6, 6, { 1233, 49, 46, 38 }, Button_Create_Archer, nullptr, NULL);
	Archer_gold_cost = App->gui->CreateGuiElement(Types::image, 1090, 140, { 690, 5, 85, 26 }, App->scene->ingameUI, nullptr, NULL);
	Archer_Text_Gold = App->gui->CreateGuiElement(Types::text, 1120, 140, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "999", App->font->smallfont);
	Archer_stone_cost = App->gui->CreateGuiElement(Types::image, 1090, 165, { 832, 5, 85, 26 }, App->scene->ingameUI, nullptr, NULL);
	Archer_Text_stone = App->gui->CreateGuiElement(Types::text, 1120, 165, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "999", App->font->smallfont);
	
}

void Human_Upgrade::DeleteUpgradeUI()
{
	if (Button_Upgrade_Footman != nullptr)
	{
		Button_Upgrade_Footman->to_delete = true;
		Swordman_gold_cost->to_delete = true;
		Swordman_Text_Gold->to_delete = true;
		Swordman_stone_cost->to_delete = true;
		Swordman_Text_stone->to_delete = true;

		Swordman_gold_cost = nullptr;
		Swordman_Text_Gold = nullptr;
		Swordman_stone_cost = nullptr;
		Swordman_Text_stone = nullptr;
		Button_Upgrade_Footman = nullptr;
	}

	if (Button_Create_Archer != nullptr)
	{
		Button_Create_Archer->to_delete = true;
		Archer_stone_cost->to_delete = true;
		Archer_Text_stone->to_delete = true;
		Archer_gold_cost->to_delete = true;
		Archer_Text_Gold->to_delete = true;

		Archer_gold_cost = nullptr;
		Archer_Text_Gold = nullptr;
		Archer_stone_cost = nullptr;
		Archer_Text_stone = nullptr;
		Button_Create_Archer = nullptr;
	}
}


void Human_Upgrade::GuiInput(GuiItem* guiElement) {
	if (guiElement == Button_Upgrade_Footman)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		if (App->scene->wood >= 999 && App->scene->gold >= 999 || App->scene->debug == true ) 
		{
			Upgrading_swordman = true;
			Swordman_Upgrade = true;
		}
		//isSelected = true;
	}
	else if (guiElement == Button_Create_Archer)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		if (App->scene->wood >= 999 && App->scene->gold >= 999 || App->scene->debug == true) {
			Archer_Upgrade = true;
			Upgrading_Archer = true;
		}
		isSelected = true;
	}

}