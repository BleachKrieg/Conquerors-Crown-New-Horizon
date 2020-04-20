#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Fonts.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "EntityRequest.h"
#include "j1Minimap.h"
#include "j1FadeToBlack.h"
#include "j1WaveSystem.h"


j1Scene::j1Scene() : j1Module()
{
	name.create("scene");

	Building_preview = false;
	active = false;
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Loading Scene");
	pugi::xml_node map;

	for (map = config.child("map"); map; map = map.next_sibling("map")) {
		p2SString lvlname;
		lvlname.create(map.attribute("name").as_string());
	}
	logoSheet_file_name = config.child("logo").attribute("file").as_string("");
	teamLogoSheet_file_name = config.child("team_logo").attribute("file").as_string("");
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = false;

	LOG("Start scene");

	current_scene = scenes::logo;
	current_level = "First level design.tmx";
	debug = false;
	wood = 0u;
	stone = 0u;
	gold = 0u;
	timer = 660;
	map_coordinates = { 0, 0 };

	//debug_tex = App->tex->Load("textures/maps/Tile_select.png");
	//App->entity->CreateEntity(DynamicEnt::DynamicEntityType::TEST_1, 100, 200);
	App->audio->PlayMusic("Audio/Music/Warcraft_II_Logo_Music.ogg");
	

	if (CreateLogo()) ret = true;

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Scene", Profiler::Color::Snow);


	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Update_Scene", Profiler::Color::Tomato);
	

	switch (current_scene) 
	{
	case scenes::menu:

		break;
	case scenes::logo:
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
			App->audio->PauseMusic(1.0f);
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}
		if (logoTimer.ReadSec() <= 5.5) {
			current_animation = &team_logo;
		}
		else {
			current_animation = &logo;
			logoTextTimer++;
		}
		break;
	case scenes::victory:
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}

		if (scale_victory < 0.005)
		{
			scale_victory = scale_victory + 0.0001;
		}
		else if (scale_victory < 0.5f)
		{
			if (speed_victory > 0.001)
			{
				speed_victory -= 0.00002;
			}
			scale_victory = scale_victory + speed_victory;
		}

		break;
	case scenes::defeat:
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}

		if (scale_defeat < 0.005)
		{
			scale_defeat = scale_defeat + 0.0001;
		}
		else if (scale_defeat < 0.5f)
		{
			if (speed_defeat > 0.001)
			{
				speed_defeat -= 0.00002;
			}
			scale_defeat = scale_defeat + speed_defeat;
		}

		break;
	case scenes::ingame:
		//Camera movement inputs
		int x, y;
		App->input->GetMousePosition(x, y);

		mouse_position = App->render->ScreenToWorld(x, y);

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			App->render->camera.y += 500 * dt;
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			App->render->camera.y -= 500 * dt;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			App->render->camera.x += 500 * dt;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			App->render->camera.x -= 500 * dt;
		}

		//Camera Limits
		if (App->render->camera.x > 0) { App->render->camera.x = 0; }
		int camera_limit_x = (-1 * App->map->data.width * App->map->data.tile_width) + App->render->camera.w;
		if (App->render->camera.x < camera_limit_x) { App->render->camera.x = camera_limit_x; }

		if (App->render->camera.y > 0) { App->render->camera.y = 0; }
		int camera_limit_y = (-1 * App->map->data.height * App->map->data.tile_height) + App->render->camera.h;
		if (App->render->camera.y < camera_limit_y) { App->render->camera.y = camera_limit_y; }
		
		//UI Position update
		ingameUIPosition = App->render->ScreenToWorld(0, 442);
		ingameUI->SetLocalPos(ingameUIPosition.x, ingameUIPosition.y);

		//Debug input
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) 
		{
			debug = !debug;
			App->map->blitColliders = !App->map->blitColliders;
		}			

		if (debug)

		{
			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::SWORDMAN, { mouse_position.x, mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::ARCHER, { mouse_position.x, mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::GATHERER, { mouse_position.x, mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::TROLL, { mouse_position.x, mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN && !Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanBarracks, mouse_position.x, mouse_position.y);
				Building_preview = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN && !Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, mouse_position.x, mouse_position.y);
				Building_preview = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
			{
				App->scene->AddResource("wood", 100);
				App->scene->AddResource("stone", +100);
				App->scene->AddResource("gold", +100);
			}
		}

		//Draw the map
		App->map->Draw();
		map_coordinates = App->map->WorldToMap(mouse_position.x, mouse_position.y);

		//Victory and Defeat scenes

		if (timer <= 0 && !finish)
		{
			App->fade->FadeToBlack(scenes::victory, 2.0f);
			finish = true;
		}
		else if (App->entity->player_stat_ent.size() == 0 && gameClock.ReadSec() > 5 && !finish)
		{
			LOG("%f %d", gameClock.ReadSec(), App->entity->player_stat_ent.size());

			App->fade->FadeToBlack(scenes::defeat, 2.0f);
			finish = true;
		}
		else {
			//gameClock Update
			timer = 660 - gameClock.ReadSec();
			TimeToClock();
		}


		break;
	}
	
	//App->render->Blit(debug_tex, p.x, p.y);

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	bool ret = true;
	switch (current_scene)
	{
	case scenes::menu:

		break;
	case scenes::ingame:

		//Mouse input for UI buttons
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
			if (App->entity->IsSomethingSelected())
			{
				if (townHallButton != nullptr) DeleteButtonsUI();
			}
			else
			{
				if (townHallButton == nullptr) CreateButtonsUI();
			}
		}

		break;
	case scenes::logo:
		if (logoTimer.ReadSec() <= 4.5) {
			SDL_SetRenderDrawColor(App->render->renderer, 20, 20, 20, 255);
			SDL_RenderFillRect(App->render->renderer, &teamLogoBackground);

			App->render->Blit(teamLogoSheet, current_animation->pivotx[current_animation->returnCurrentFrame()], current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)));
		}
		else if (logoTimer.ReadSec() <= 5.5) {
			SDL_SetRenderDrawColor(App->render->renderer, 20, 20, 20, 255);
			SDL_RenderFillRect(App->render->renderer, &teamLogoBackground);

			App->render->Blit(teamLogoSheet, current_animation->pivotx[current_animation->returnCurrentFrame()], current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)));

			SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, alpha);
			SDL_RenderFillRect(App->render->renderer, &teamLogoBackground);
			alpha += 10;
			if (alpha > 255) { alpha = 255; }
		}
		else {
			App->render->Blit(logoSheet, 220 + current_animation->pivotx[current_animation->returnCurrentFrame()], 200 + current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)));
			
			SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, alpha);
			SDL_RenderFillRect(App->render->renderer, &teamLogoBackground);
			alpha -= 5;
			if (alpha < 0) { alpha = 0; }
		}
		break;

	case scenes::victory:
		
		App->render->Blit(victoryLogo, ((App->render->camera.w/2)/ scale_victory)-(App->scene->rect_victory.w*scale_victory), ((App->render->camera.h / 2) / scale_victory) - (rect_victory.h*scale_victory) - 150, &rect_victory, 1.0f, 1.0f, SDL_FLIP_NONE, scale_victory);

		break;

	case scenes::defeat:
		
		App->render->Blit(defeatLogo, ((App->render->camera.w / 2) / scale_defeat) - (rect_defeat.w*scale_defeat), ((App->render->camera.h / 2) / scale_defeat) - (rect_defeat.h*scale_defeat) - 150, &rect_defeat, 1.0f, 1.0f, SDL_FLIP_NONE, scale_defeat);

		break;

	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	App->map->CleanUp();
		
	LOG("Freeing scene");

	return true;
}
bool j1Scene::Load(pugi::xml_node& data)
{
	LOG("Loading Scene state");
	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{
	LOG("Saving Scene state");
	

	pugi::xml_node scene = data.append_child("scenename");
	scene.append_attribute("name") = current_level.GetString();

	return true;
}

void j1Scene::LoadTiledEntities() {

	list<MapLayer*>::iterator Layer_list;
	MapLayer* layer;

	for (Layer_list = App->map->data.layers.begin(); Layer_list != App->map->data.layers.end(); ++Layer_list)
	{
		layer = *Layer_list;
		LOG("%s, %d", layer->name.GetString(), layer->returnPropValue("Navigation"));
		if (layer->returnPropValue("Navigation") == 2)
		{
			for (int y = 0; y < App->map->data.height; ++y)
			{
				for (int x = 0; x < App->map->data.width; ++x)
				{
					int tile_id = layer->Get(x, y);

					if (tile_id != 0)
					{
						iPoint pos;
						pos = App->map->MapToWorld(x, y);
						switch (tile_id) {
						case 401:
							active = true;
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall,pos.x, pos.y);
							break;
						case 418:
							active = true;
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanBarracks, pos.x, pos.y);
							break;
						}
						if (tile_id >= 102 && tile_id <= 141 && tile_id != 126)
						{
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::Resource, pos.x, pos.y, 1u);
						}
						if (tile_id >= 373 && tile_id <= 375)
						{
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::Resource, pos.x, pos.y, 3u);
						}
					}
				}
			}
		}
	}
	active = false;
}

void j1Scene::DeleteScene() {
	//Deleting scene
	switch (current_scene)
	{
	case scenes::menu:
		DeleteUI();
		break;
	case scenes::ingame:
		DeleteUI();
		App->entity->DeleteAllEntities();
		App->minimap->CleanUp();
		App->map->CleanUp();
		App->wave->wave_ongoing = false;
		break;
	case scenes::logo:
		DeleteUI();
		break;
	case scenes::victory:
		App->tex->UnLoad(victoryLogo);
		DeleteUI();
		break;
	case scenes::defeat:
		App->tex->UnLoad(defeatLogo);
		DeleteUI();
		break;

	}

}

void j1Scene::CreateScene(scenes next_scene) {
	//Creating scene
	switch (next_scene)
	{
	case scenes::menu:
		current_scene = scenes::menu;
		CreateMenu();
		App->audio->StopFx();
		App->audio->PlayMusic("Audio/Music/Warcraft_II_Main_Menu.ogg", 2.0F);
		break;
	case scenes::ingame:
		current_scene = scenes::ingame;
		CreateInGame();
		App->audio->PlayMusic("Audio/Music/Human/Human_Battle_1.ogg", 2.0F);
		App->render->camera.x = -2830;
		App->render->camera.y = -967;
		gameClock.Start();
		finish = false;
		break;
	case scenes::logo:
		current_scene = scenes::logo;
		CreateLogo();
		break;
	case scenes::victory:
		current_scene = scenes::victory;
		CreateVictory();
		break;
	case scenes::defeat:
		current_scene = scenes::defeat;
		CreateDefeat();
		break;
	}
}

bool j1Scene::CreateMenu() {
	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	//Loading UI
	SDL_Rect rect = { 0, 500, 1280, 720 };

	menuBackground = App->gui->CreateGuiElement(Types::image, 0, 0, rect);

	menuButtonNewGame = App->gui->CreateGuiElement(Types::button, 500, 300, { 0, 63, 303, 42 }, menuBackground, this, NULL);
	menuButtonNewGame->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	menuTextNewGame = App->gui->CreateGuiElement(Types::text, 65, 4, { 0, 0, 138, 30 }, menuButtonNewGame, nullptr, "New Game");

	menuButtonLoadGame = App->gui->CreateGuiElement(Types::button, 500, 350, { 0, 63, 303, 42 }, menuBackground, this, NULL);
	menuButtonLoadGame->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	menuTextLoadGame = App->gui->CreateGuiElement(Types::text, 63, 4, { 0, 0, 138, 30 }, menuButtonLoadGame, nullptr, "Load Game");

	menuButtonOptions = App->gui->CreateGuiElement(Types::button, 500, 400, { 0, 63, 303, 42 }, menuBackground, this, NULL);
	menuButtonOptions->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	menuTextOptions = App->gui->CreateGuiElement(Types::text, 90, 4, { 0, 0, 138, 30 }, menuButtonOptions, nullptr, "Options");

	menuButtonExit = App->gui->CreateGuiElement(Types::button, 500, 450, { 0, 63, 303, 42 }, menuBackground, this, NULL);
	menuButtonExit->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	menuTextExit = App->gui->CreateGuiElement(Types::text, 115, 4, { 0, 0, 138, 30 }, menuButtonExit, nullptr, "Exit");


	return true;
}

bool j1Scene::CreateInGame() 
{
	bool ret = true;

	//Loading the map
	if (App->map->Load(current_level.GetString()) == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
		{
			LOG("Setting map %d", data[1]);
			App->pathfinding->SetMap(w, h, data);
		}
		else
		{
			LOG("Could not create walkability");
			ret = false;
		}
		RELEASE_ARRAY(data);
	}
	//Creating minimap
	if (ret) ret = App->minimap->Start();

	//Loading UI
	SDL_Rect downRect = { 0, 222, 1280, 278 };
	SDL_Rect topRect = { 0, 0, 1280, 49 };
	ingameUI = App->gui->CreateGuiElement(Types::image, 0, 442, downRect);
	ingameTopBar = App->gui->CreateGuiElement(Types::image, 0, -442, topRect, ingameUI);

	ingameButtonMenu = App->gui->CreateGuiElement(Types::button, 100, 3, { 0, 150, 138, 30 }, ingameTopBar, this, NULL);
	ingameButtonMenu->setRects({ 139, 150, 138, 30 }, { 0, 181, 138, 30 });
	ingameTextMenu = App->gui->CreateGuiElement(Types::text, 33, 4, { 0, 0, 138, 30 }, ingameButtonMenu, nullptr, "Menu", App->font->smallfont);

	ingameTextGold = App->gui->CreateGuiElement(Types::text, 722, 7, { 0, 0, 138, 30 }, ingameTopBar, nullptr, "0", App->font->smallfont);
	ingameTextWood = App->gui->CreateGuiElement(Types::text, 862, 7, { 0, 0, 138, 30 }, ingameTopBar, nullptr, "0", App->font->smallfont);
	ingameTextStone = App->gui->CreateGuiElement(Types::text, 1003, 7, { 0, 0, 138, 30 }, ingameTopBar, nullptr, "0", App->font->smallfont);
	ingameTextClock = App->gui->CreateGuiElement(Types::text, 475, 7, { 0, 0, 138, 30 }, ingameTopBar, nullptr, "00:00", App->font->smallfont);

	LoadTiledEntities();

	if(ret) ret = CreateButtonsUI();

	App->wave->wave_ended.Start();
	App->wave->wave_ongoing = false;

	return ret;
}

bool j1Scene::CreateButtonsUI()
{
	townHallButton = App->gui->CreateGuiElement(Types::button, 1000, 80, { 306, 125, 58, 50 }, ingameUI, this, NULL);
	townHallButton->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
	townHallImage = App->gui->CreateGuiElement(Types::image, 6, 6, { 1092, 49, 46, 38 }, townHallButton, nullptr, NULL);

	return true;
}

bool j1Scene::DeleteButtonsUI()
{
	townHallButton->to_delete = true;
	townHallButton = nullptr;

	return true;
}

bool j1Scene::CreateLogo() {
	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	teamLogoSheet = App->tex->Load(teamLogoSheet_file_name.GetString());

	TeamLogoPushbacks();

	teamLogoBackground.x = 0;
	teamLogoBackground.y = 0;
	teamLogoBackground.w = 1280;
	teamLogoBackground.h = 720;

	logoTimer.Start();

	logoTextTimer = 0;
	alpha = 0;

	logoSheet = App->tex->Load(logoSheet_file_name.GetString());
	
	LogoPushbacks();

	//Loading UI
	SDL_Rect rect = { 1280, 0, 1280, 720 };

	logoBackground = App->gui->CreateGuiElement(Types::image, 0, 0, rect);

	logoTextClick = App->gui->CreateGuiElement(Types::text, 450, 520, { 0, 0, 138, 30 }, logoBackground, nullptr, "Press X to continue..");

	App->audio->PlayFx(1, App->audio->Logo_FX, 0);

	return true;
}

bool j1Scene::CreateVictory() {
	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	scale_victory = 0.0f;
	speed_victory = 0.005f;

	victoryLogo = App->tex->Load("textures/gui/VictorySheet.png");

	//Loading UI
	SDL_Rect rect = { 1280, 0, 1280, 720 };

	victoryBackground = App->gui->CreateGuiElement(Types::image, 0, 0, rect);	

	//App->render->Blit(victoryLogo, 0, 0);

	victoryButtonContinue = App->gui->CreateGuiElement(Types::button, 480, 550, { 0, 63, 303, 42 }, victoryBackground, this, NULL);
	victoryButtonContinue->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	victoryTextContinue = App->gui->CreateGuiElement(Types::text, 75, 4, { 0, 0, 138, 30 }, victoryButtonContinue, nullptr, "Continue");
	

	//uncomment that to use text and not button to continue
	//victoryTextClick = App->gui->CreateGuiElement(Types::text, 450, 520, { 0, 0, 138, 30 }, victoryBackground, nullptr, "Press X to continue..");

	//victory music
	App->audio->PlayMusic("Audio/Music/Human/Human_Victory.ogg", 2.0F);


	return true;
}

bool j1Scene::CreateDefeat() {
	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	scale_defeat = 0.0f;
	speed_defeat = 0.005f;

	defeatLogo = App->tex->Load("textures/gui/DefeatSheet.png");

	//Loading UI
	SDL_Rect rect = { 1280, 0, 1280, 720 };

	defeatBackground = App->gui->CreateGuiElement(Types::image, 0, 0, rect);

	//App->render->Blit(victoryLogo, 0, 0);

	defeatButtonContinue = App->gui->CreateGuiElement(Types::button, 480, 550, { 0, 63, 303, 42 }, defeatBackground, this, NULL);
	defeatButtonContinue->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	defeatTextContinue = App->gui->CreateGuiElement(Types::text, 75, 4, { 0, 0, 138, 30 }, defeatButtonContinue, nullptr, "Continue");


	//uncomment that to use text and not button to continue
	//victoryTextClick = App->gui->CreateGuiElement(Types::text, 450, 520, { 0, 0, 138, 30 }, victoryBackground, nullptr, "Press X to continue..");

	//victory music
	App->audio->PlayMusic("Audio/Music/Human/Human_Defeat.ogg", 2.0F);


	return true;
}

bool j1Scene::DeleteUI() 
{
	menuBackground = nullptr;
	menuButtonNewGame = nullptr;
	menuTextNewGame = nullptr;
	menuButtonLoadGame = nullptr;
	menuTextLoadGame = nullptr;
	menuButtonOptions = nullptr;
	menuTextOptions = nullptr;
	menuButtonExit = nullptr;
	menuTextExit = nullptr;
	ingameUI = nullptr;
	ingameTopBar = nullptr;
	ingameButtonMenu = nullptr;
	ingameTextMenu = nullptr;
	ingameTextGold = nullptr;
	ingameTextWood = nullptr;
	ingameTextStone = nullptr;
	ingameTextClock = nullptr;
	logoTextClick = nullptr;
	logoBackground = nullptr;

	//delete victory scene
	victoryBackground = nullptr;
	victoryButtonContinue = nullptr;
	victoryTextContinue = nullptr;
	victoryTextClick = nullptr;
	//delete defeat scene
	defeatBackground = nullptr;
	defeatButtonContinue = nullptr;
	defeatTextContinue = nullptr;
	defeatTextClick = nullptr;

	App->tex->UnLoad(logoSheet);
	App->tex->UnLoad(teamLogoSheet);
	App->gui->DeleteAllGui();
	return true;
}

void j1Scene::GuiInput(GuiItem* guiElement) {
	//Menu buttons
	if (guiElement == menuButtonNewGame) {
		App->audio->PlayFx(-1, App->audio->click_to_play, 0);
		App->audio->PauseMusic(1.0f);
		App->fade->FadeToBlack(scenes::ingame, 2.0f);
	}
	else if (guiElement == menuButtonExit) {
		App->audio->PlayFx(-1, App->audio->click_to_play, 0);
		App->quitGame = true;
	}
	else if (guiElement == menuButtonLoadGame) {
		App->audio->PlayFx(-1, App->audio->click_to_play, 0);
	}
	else if (guiElement == menuButtonOptions) {
		App->audio->PlayFx(-1, App->audio->click_to_play, 0);
	}


	//InGame Buttons
	if (guiElement == ingameButtonMenu) {
		App->audio->PlayFx(-1, App->audio->click_to_play, 0);
		App->audio->PauseMusic(1.0f);
		App->fade->FadeToBlack(scenes::menu, 2.0f);
	}
	else if (guiElement == townHallButton) {
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		if (!Building_preview && !App->entity->IsSomethingSelected())
		{
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, mouse_position.x, mouse_position.y);
			Building_preview = true;
		}
	}

	//Victory Buttons
	if (guiElement == victoryButtonContinue) {
		App->audio->PlayFx(-1, App->audio->click_to_play, 0);
		App->fade->FadeToBlack(scenes::menu, 2.0f);
	}

	//Defeat Buttons
	if (guiElement == defeatButtonContinue) {
		App->audio->PlayFx(-1, App->audio->click_to_play, 0);
		App->fade->FadeToBlack(scenes::menu, 2.0f);
	}
}

void j1Scene::AddResource(char* typeResource, int quantity) 
{
	int i = 0;
	if (typeResource == "gold")i = 1;
	else if (typeResource == "wood")i = 2;
	else if (typeResource == "stone")i = 3;

	switch (i) {
	case 1:
		gold += quantity;
		ingameTextGold->SetText(to_string(gold).c_str());
		break;
	case 2:
		wood += quantity;
		ingameTextWood->SetText(to_string(wood).c_str());
		break;
	case 3:
		stone += quantity;
		ingameTextStone->SetText(to_string(stone).c_str());
		break;
	case 0:
		LOG("The parameter in AddResource is not correct.");
		break;
	}
}

void j1Scene::TimeToClock() 
{
	mins = to_string(timer/60);

	secs = to_string(timer % 60);

	if (mins.size() < 2) mins.insert(0, "0");
	if (secs.size() < 2) secs.insert(0, "0");

	string str = mins + ":" + secs;

	ingameTextClock->SetText(str.c_str());
}

//Animations

void j1Scene::LogoPushbacks() {

	logo.PushBack({913, 44, 828, 295}, 0.04f, 0, 0, 0, 0);
	logo.PushBack({ 1811, 44, 828, 295 }, 0.18f, 0, 0, 0, 0);
	logo.PushBack({ 2709, 44, 828, 295 }, 0.25f, 0, 1, 0, 0);
	logo.PushBack({ 15, 439, 828, 295 }, 0.25f, 3, 0, 0, 0);
	logo.PushBack({ 913, 439, 828, 295 }, 0.25f, 3, 0, 0, 0);
	logo.PushBack({ 1811, 439, 828, 295 }, 0.25f, 3, 0, 0, 0);
	logo.PushBack({ 2709, 439, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 15, 845, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 913, 845, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 1811, 845, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 2709, 845, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 15, 1240, 828, 295 }, 0.18f, 0, 0, 0, 0);
	logo.PushBack({ 913, 1240, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 1811, 1240, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 2709, 1240, 828, 295 }, 0.25f, 0, 0, 0, 0);
	logo.PushBack({ 15, 1635, 828, 295 }, 0.15f, 0, 0, 0, 0);
	logo.PushBack({ 1811, 1635, 828, 295 }, 0.45f, 0, 0, 0, 0);
	logo.PushBack({ 913, 1635, 828, 295 }, 0.035f, 3, 0, 0, 0);
	logo.PushBack({ 1811, 1635, 828, 295 }, 0.55f, 0, 0, 0, 0);
	logo.PushBack({ 15, 44, 828, 295 }, 0.1f, 0, 0, 0, 0);

	logo.loop = false;
}

void j1Scene::TeamLogoPushbacks() {

	team_logo.PushBack({ 0, 875, 10, 10 }, 0.02f, 0, 0, 0, 0);
	team_logo.PushBack({0, 0, 259, 423}, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 286, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 572, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 858, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 1144, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 1430, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 1716, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 2002, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 2288, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 2574, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 2860, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 3146, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 3432, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 3718, 0, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 0, 445, 259, 423 }, 0.35f, 510, 148, 0, 0);
	team_logo.PushBack({ 286, 445, 259, 423 }, 0.15f, 510, 148, 0, 0);
	team_logo.PushBack({ 572, 445, 1280, 720 }, 0.3f, 0, 0, 0, 0);
	team_logo.PushBack({ 1879, 445, 1280, 720 }, 0.5f, 0, 0, 0, 0);
	team_logo.PushBack({ 572, 445, 1280, 720 }, 0.25f, 0, 0, 0, 0);
	team_logo.PushBack({ 0, 1187, 1280, 720 }, 0.4f, 0, 0, 0, 0);
	team_logo.PushBack({ 1311, 1187, 1280, 720 }, 0.4f, 0, 0, 0, 0);
	team_logo.PushBack({ 2622, 1187, 1280, 720 }, 0.4f, 0, 0, 0, 0);
	team_logo.PushBack({ 3335, 445, 712, 720 }, 0.2f, 356, 0, 0, 0);

	team_logo.loop = false;
}
