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
#include "j1CutsceneManager.h"
#include "j1Tutorial.h"
#include "FoWManager.h"
#include "j1Video.h"
#include "J1GroupMov.h"


j1Scene::j1Scene() : j1Module()
{
	name.create("scene");

	Building_preview = false;
	active = false;
	logo_team_sfx_counter = 0;
	win_lose_counter = 0;
	UiEnabled = true;
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
	fullscreen = false;
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = false;

	LOG("Start scene");

	current_scene = scenes::logo;
	current_level = "Tutorial.tmx";
	debug = false;
	wood = 0u;
	stone = 0u;
	gold = 0u;
	timer = 660;
	map_coordinates = { 0, 0 };
	optionsMenu = false;

	upgrade_swordman = 0;
	upgrade_archer = 0;
	upgrade_knight = 0;

	

	//App->audio->PlayFx(1, App->audio->intro_fx, 0);

	intro_video = App->video->Load("Assets/video/team-logo.ogv", App->render->renderer);
	loop = true;

	wall_create = false;

	wants_to_load = false;

	//UI elements
	menuButtonNewGame = nullptr;
	menuTextNewGame = nullptr;
	menuButtonLoadGame = nullptr;
	menuTextLoadGame = nullptr;
	menuButtonOptions = nullptr;
	menuTextOptions = nullptr;
	menuButtonExit = nullptr;
	menuTextExit = nullptr;
	menuBackground = nullptr;

	ingameUI = nullptr;
	ingameTopBar = nullptr;
	ingameButtonMenu = nullptr;
	ingameTextMenu = nullptr;
	ingameTextGold = nullptr;
	ingameTextWood = nullptr;
	ingameTextStone = nullptr;
	ingameTextClock = nullptr;
	ingameTextWave = nullptr;

	townHallButton = nullptr;
	townHallImage = nullptr;
	townHallWoodCostImage = nullptr;
	townHallStoneCostImage = nullptr;
	townHallWoodCostText = nullptr;
	townHallStoneCostText = nullptr;

	optionsBackground = nullptr;
	optionsTitleText = nullptr;
	optionsButtonClose = nullptr;
	optionsTextClose = nullptr;
	optionsMusicText = nullptr;
	optionsMusicSlider = nullptr;
	optionsFxText = nullptr;
	optionsFxSlider = nullptr;
	optionsButtonFullScreen = nullptr;
	optionsTextFullScreen = nullptr;

	pausemenuBackground = nullptr;
	pausemenuButtonResume = nullptr;
	pausemenuTextResume = nullptr;
	pausemenuButtonOptions = nullptr;
	pausemenuTextOptions = nullptr;
	pausemenuButtonSave = nullptr;
	pausemenuTextSave = nullptr;
	pausemenuButtonLoad = nullptr;
	pausemenuTextLoad = nullptr;
	pausemenuButtonExit = nullptr;
	pausemenuTextExit = nullptr;

	logoTextClick = nullptr;
	logoBackground = nullptr;

	victoryBackground = nullptr;
	victoryButtonContinue = nullptr;
	victoryTextContinue = nullptr;
	victoryTextClick = nullptr;

	defeatBackground = nullptr;
	defeatButtonContinue = nullptr;
	defeatTextContinue = nullptr;
	defeatTextClick = nullptr;

	//debug_tex = App->tex->Load("textures/maps/Tile_select.png");
	//App->entity->CreateEntity(DynamicEnt::DynamicEntityType::TEST_1, 100, 200);
	App->audio->PlayMusic("Warcraft_II_Logo_Music.ogg");
	
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
	
	last_dt = dt;

	switch (current_scene) 
	{
	case scenes::menu:
		
		break;
	case scenes::tutorial:
		if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) 
		{
			App->fade->FadeToBlack(scenes::ingame, 2.0f);
		}
		//UI Position update
		ingameUIPosition = App->render->ScreenToWorld(0, 442);
		ingameUI->SetLocalPos(ingameUIPosition.x, ingameUIPosition.y);
		App->map->Draw();
		break;
	case scenes::logo:
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
			App->audio->PauseMusic(1.0f);
			App->audio->PlayFx(2, App->audio->click_to_play, 0);

			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}
				
		if (logoTimer.ReadSec() <= 5.5) {
			current_animation = &team_logo;
			logo_team_sfx_counter++;
			if (logo_team_sfx_counter == 150) {
				App->audio->PlayFx(2, App->audio->logo_team_fx, 0);
			}

		}
		
		else {
			logo_team_sfx_counter = 0;
			current_animation = &logo;
			logoTextTimer++;
			if (logoTextTimer == 88) {
				App->audio->PlayFx(1, App->audio->logo_game_fx, 0);
			}
	//		LOG("Logo text timer: %i", logoTextTimer);
		}

	//	LOG("Logo timer: %.2f", logoTimer.ReadSec());
		break;
	case scenes::victory:
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}
		

		/*if (scale_victory < 0.005)
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
		}*/

		break;
	case scenes::defeat:
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}
		
		/*if (scale_defeat < 0.005)
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
		}*/

		break;
	case scenes::ingame:
		//Camera movement inputs
		int x, y;
		App->input->GetMousePosition(x, y);

		mouse_position = App->render->ScreenToWorld(x, y);

		if (!pauseMenu && App->cutscene->cinematic_camera.active == false)
		{
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
				App->render->camera.y += 500 * dt;
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
				App->render->camera.y -= 500 * dt;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				App->render->camera.x += 500 * dt;
			}

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				App->render->camera.x -= 500 * dt;
			}
		}
		//Camera Limits
		if (App->render->camera.x > 0) App->render->camera.x = 0;
		int camera_limit_x = (-1 * App->map->data.width * App->map->data.tile_width) + App->render->camera.w;
		if (App->render->camera.x < camera_limit_x) App->render->camera.x = camera_limit_x;

		if (App->render->camera.y > 0) App->render->camera.y = 0;
		int camera_limit_y = (-1 * App->map->data.height * App->map->data.tile_height) + App->render->camera.h;
		if (App->render->camera.y < camera_limit_y) App->render->camera.y = camera_limit_y;
		
		//UI Position update
		ingameUIPosition = App->render->ScreenToWorld(0, 442);
		ingameUI->SetLocalPos(ingameUIPosition.x, ingameUIPosition.y);

		//Pause Menu
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			if (!pauseMenu) {

				CreatePauseMenu();
			}
			else {
				DeletePauseMenu();
			}
			pauseMenu = !pauseMenu;
		}

		//Debug input
		if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		{
			App->SaveGame();
		}
		if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		{
			if (App->CheckSaveGame()) {
				
				App->fade->FadeToBlack(scenes::ingame, 2.0f);
				wants_to_load = true;
				//App->LoadGame();

			}
		}

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
			if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::KNIGHT, { mouse_position.x, mouse_position.y });
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
			if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::OGRE, { mouse_position.x, mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::GRUNT, { mouse_position.x, mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN && !Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanBarracks, mouse_position.x, mouse_position.y);
				Building_preview = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN && !Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, mouse_position.x, mouse_position.y);
				Building_preview = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
			{
				App->scene->AddResource("wood", 100);
				App->scene->AddResource("stone", 100);
				App->scene->AddResource("gold", 100);
			}
			if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::enemy_barrack, mouse_position.x, mouse_position.y);
			}

			/*if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
				App->fade->FadeToBlack(scenes::defeat, 2.0f);
			}
			if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
				App->fade->FadeToBlack(scenes::victory, 2.0f);
			}*/
			if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN && !Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanUpgrade, mouse_position.x, mouse_position.y);
				Building_preview = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && !Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanWall, mouse_position.x, mouse_position.y);
				Building_preview = true;
			}
		}

		if (stone >= 100 && wall_create == true && !Building_preview)
		{
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanWall, mouse_position.x, mouse_position.y);
			Building_preview = true;

			if(App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::enemy_barrack, mouse_position.x, mouse_position.y);

		}

		//Draw the map
		App->map->Draw();
		map_coordinates = App->map->WorldToMap(mouse_position.x, mouse_position.y);

		int townhall_counter = 0;
		for (int i = 0; i < App->entity->player_stat_ent.size(); i++)
		{
			if (App->entity->player_stat_ent[i]->name == "town_hall")
				townhall_counter++;
		}

		if (timer <= 0 && !finish)
		{
			App->fade->FadeToBlack(scenes::victory, 2.0f);
			win_lose_counter = 0;
			finish = true;
		}
		else if (gameClock.ReadSec() > 5 && !finish && townhall_counter == 0)
		{
	//		LOG("%f %d", gameClock.ReadSec(), App->entity->player_stat_ent.size());
				App->fade->FadeToBlack(scenes::defeat, 2.0f);
				win_lose_counter = 0;
				finish = true;
		}
		else {
			//gameClock Update
			timer = 660 - gameClock.ReadSec() - time_loaded;
			if (Cooldown.ReadSec() > 1)
			{
				TimeToClock();
				Cooldown.Start();
			}
		}
		break;
	}
	
	// testing winlose scenes
	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN && debug) {
		App->fade->FadeToBlack(scenes::defeat, 2.0f);
		win_lose_counter = 0;
	}
	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN && debug) {
		App->fade->FadeToBlack(scenes::victory, 2.0f);
		win_lose_counter = 0;
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
	case scenes::tutorial:
		//portraits
		if (App->movement->portrait_entity != nullptr)
		{

			if (App->movement->portrait_entity->name == p2SString("town_hall"))
			{
				SDL_Rect rect = { 3126, 312, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_barracks"))
			{
				SDL_Rect rect = { 3126, 468, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_footman"))
			{
				SDL_Rect rect = { 2978, 156, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_gatherer"))
			{
				SDL_Rect rect = { 2978, 0, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_archer"))
			{
				SDL_Rect rect = { 3126, 0, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_upgrade"))
			{
				SDL_Rect rect = { 3126, 624, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_knight"))
			{
				SDL_Rect rect = { 3126, 156, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}

		}
		break;
	case scenes::ingame:
		//portraits
		if (App->movement->portrait_entity != nullptr)
		{

			if (App->movement->portrait_entity->name == p2SString("town_hall"))
			{
				SDL_Rect rect = { 3126, 312, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_barracks"))
			{
				SDL_Rect rect = { 3126, 468, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_footman"))
			{
				SDL_Rect rect = { 2978, 156, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_gatherer"))
			{
				SDL_Rect rect = { 2978, 0, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_archer"))
			{
				SDL_Rect rect = { 3126, 0, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_upgrade"))
			{
				SDL_Rect rect = { 3126, 624, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}
			if (App->movement->portrait_entity->name == p2SString("human_knight"))
			{
				SDL_Rect rect = { 3126, 156, 144, 152 };
				App->render->Blit(App->gui->GetAtlas(), App->render->viewport.x + 332, App->render->viewport.y + 529, &rect, 0, 0);
			}

		}
		//Mouse input for UI buttons
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
			if (App->entity->IsSomethingSelected())
			{
				if (townHallButton != nullptr) ret = DeleteButtonsUI();
			}
			else
			{
				if (townHallButton == nullptr) ret = CreateButtonsUI();
			}
		}
		break;
	case scenes::logo:
		if (logoTimer.ReadSec() <= 6.5) {
			SDL_SetRenderDrawColor(App->render->renderer, 20, 20, 20, 255);
			SDL_RenderFillRect(App->render->renderer, &teamLogoBackground);

			//video
			if (intro_video != 0)
			{
				video_texture = App->video->UpdateVideo(intro_video);

				App->render->Blit(video_texture, 0, 0);

				if (App->video->IsPlaying(intro_video) == 0)
				{
					App->video->DestroyVideo(intro_video);
					intro_video = 0;
				}
			}


			if (intro_video == 0 && loop)
			{
				intro_video = App->video->Load("Assets/video/team-logo.ogv", App->render->renderer);


			}

			if (!loop)
			{

				App->render->Blit(videologo_tex, 70, -130, &loader->GetCurrentFrame(last_dt), 1.0f, 0.0f);

			}
		}
		else if (logoTimer.ReadSec() <= 7.0) {
			SDL_SetRenderDrawColor(App->render->renderer, 20, 20, 20, 255);
			SDL_RenderFillRect(App->render->renderer, &teamLogoBackground);

			//App->render->Blit(video_texture, current_animation->pivotx[current_animation->returnCurrentFrame()], current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)));

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
		//App->render->Blit(victoryLogo, ((App->render->camera.w/2)/ scale_victory)-(App->scene->rect_victory.w*scale_victory), ((App->render->camera.h / 2) / scale_victory) - (rect_victory.h*scale_victory) - 150, &rect_victory, 1.0f, 1.0f, SDL_FLIP_NONE, scale_victory);
		//video
		if (intro_video != 0)
		{
			video_texture = App->video->UpdateVideo(intro_video);

			App->render->Blit(video_texture, 0, 0);

			if (App->video->IsPlaying(intro_video) == 0)
			{
				App->video->DestroyVideo(intro_video);
				intro_video = 0;
			}
		}


		if (intro_video == 0 && loop)
		{
			intro_video = App->video->Load("Assets/video/victory.ogv", App->render->renderer);


		}

		if (!loop)
		{

			App->render->Blit(videologo_tex, 70, -130, &loader->GetCurrentFrame(last_dt), 1.0f, 0.0f);

		}
		break;

	case scenes::defeat:
		//App->render->Blit(defeatLogo, ((App->render->camera.w / 2) / scale_defeat) - (rect_defeat.w*scale_defeat), ((App->render->camera.h / 2) / scale_defeat) - (rect_defeat.h*scale_defeat) - 150, &rect_defeat, 1.0f, 1.0f, SDL_FLIP_NONE, scale_defeat);
		//video
		if (intro_video != 0)
		{
			video_texture = App->video->UpdateVideo(intro_video);

			App->render->Blit(video_texture, 0, 0);

			if (App->video->IsPlaying(intro_video) == 0)
			{
				App->video->DestroyVideo(intro_video);
				intro_video = 0;
			}
		}

		if (intro_video == 0 && loop)
		{
			intro_video = App->video->Load("Assets/video/defeat.ogv", App->render->renderer);

		}

		if (!loop)
		{
			App->render->Blit(videologo_tex, 70, -130, &loader->GetCurrentFrame(last_dt), 1.0f, 0.0f);

		}
		break;
	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	App->map->CleanUp();
	bool ret = true;
	ret = App->tex->UnLoad(video_texture);
	ret = App->tex->UnLoad(videologo_tex);
	loader = nullptr;
	App->minimap->CleanUp();
	LOG("Freeing scene");

	return ret;
}
bool j1Scene::Load(pugi::xml_node& data)
{
	LOG("Loading Scene state");
	gold = 0;
	wood = 0;
	stone = 0;
	AddResource("gold", data.child("resources").attribute("gold").as_int());
	AddResource("wood", data.child("resources").attribute("wood").as_int());
	AddResource("stone", data.child("resources").attribute("stone").as_int());
	time_loaded = data.child("clock").attribute("time_passed").as_int();
	upgrade_swordman = data.child("upgrades").attribute("swordman").as_int();
	upgrade_archer = data.child("upgrades").attribute("archer").as_int();
	upgrade_knight = data.child("upgrades").attribute("knight").as_int();

	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{
	LOG("Saving Scene state");
	

	pugi::xml_node scenename = data.append_child("scenename");
	scenename.append_attribute("name") = current_level.GetString();
	pugi::xml_node resources = data.append_child("resources");
	resources.append_attribute("gold") = gold;
	resources.append_attribute("wood") = wood;
	resources.append_attribute("stone") = stone;
	pugi::xml_node clock = data.append_child("clock");
	clock.append_attribute("time_passed") = gameClock.ReadSec() - time_loaded;;
	pugi::xml_node upgrades = data.append_child("upgrades");
	upgrades.append_attribute("swordman") = upgrade_swordman;
	upgrades.append_attribute("archer") = upgrade_archer;
	upgrades.append_attribute("knight") = upgrade_knight;

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
						case 381:
							active = true;
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::GoldMine, pos.x, pos.y, 0u, 50u);
							break;
						case 401:
							active = true;
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall,pos.x, pos.y);
							break;
						case 418:
							active = true;
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanBarracks, pos.x, pos.y);
							break;
						case 422:
							if (current_scene == scenes::tutorial) {
								App->entity->CreateStaticEntity(StaticEnt::StaticEntType::GoldMine, pos.x, pos.y, 0u, 80u);
							}
						}
						if (tile_id >= 102 && tile_id <= 141 && tile_id != 126)
						{
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::Resource, pos.x, pos.y, 1u);
						}
						if (tile_id >= 374 && tile_id <= 376)
						{
							App->entity->CreateStaticEntity(StaticEnt::StaticEntType::Resource, pos.x, pos.y, 2u);
						}
					}
				}
			}
		}
	}
	active = false;

	if (current_scene == scenes::ingame)
	{
		App->requests->AddRequest(Petition::SPAWN, 1.f, SpawnTypes::GATHERER, { 3520, 1175 });
		App->requests->AddRequest(Petition::SPAWN, 1.f, SpawnTypes::GATHERER, { 3520, 1165 });
		App->requests->AddRequest(Petition::SPAWN, 1.f, SpawnTypes::GATHERER, { 3520, 1185 });
	}
}

void j1Scene::DeleteScene() {
	//Deleting scene
	switch (current_scene)
	{
	case scenes::menu:
		DeleteUI();
		break;
	case scenes::tutorial:
		App->render->camera.x = 0;
		App->render->camera.y = 0;
		App->minimap->CleanUp();
		App->map->CleanUp();
		DeleteUI();
		App->entity->DeleteAllEntities();
		break;
	case scenes::ingame:
		DeleteUI();
		App->entity->DeleteAllEntities();
		App->minimap->CleanUp();
		App->map->CleanUp();
		App->fowManager->DeleteFoWMap();
		App->wave->wave_ongoing = false;
		break;
	case scenes::logo:
		App->tex->UnLoad(video_texture);
		App->tex->UnLoad(videologo_tex);
		loader = nullptr;
		DeleteUI();
		break;
	case scenes::victory:
		App->tex->UnLoad(victoryLogo);
		App->tex->UnLoad(video_texture);
		App->tex->UnLoad(videologo_tex);
		loader = nullptr;
		DeleteUI();
		break;
	case scenes::defeat:
		App->tex->UnLoad(defeatLogo);
		App->tex->UnLoad(video_texture);
		App->tex->UnLoad(videologo_tex);
		loader = nullptr;
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
		App->audio->PlayMusic("Warcraft_II_Main_Menu.ogg", 2.0F);
		break;
	case scenes::tutorial:
		upgrade_swordman = 0;
		upgrade_archer = 0;
		upgrade_knight = 0;
		current_scene = scenes::tutorial;
		CreateTutorial();
		App->audio->PlayMusic("Human/Human_Battle_5.ogg", 2.0F);
		wood = 0u;
		stone = 0u;
		gold = 0u;
		App->render->camera.x = 0;
		App->render->camera.y = -1136;
		finish = false;
		break;
	case scenes::ingame:
		upgrade_swordman = 0;
		upgrade_archer = 0;
		upgrade_knight = 0;
		current_scene = scenes::ingame;
		CreateInGame();
		App->minimap->input = true;
		App->audio->PlayMusic("Human/Human_Battle_1.ogg", 2.0F);
		App->render->camera.x = -2830;
		App->render->camera.y = -967;
		App->wave->Start();
		gameClock.Start();
		Cooldown.Start();
		timer = 660;
		wood = 0u;
		stone = 0u;
		gold = 0u;
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

bool j1Scene::CreateTutorial()
{
	App->tutorial->createUI = true;
	bool ret = true;
	TutorialTimer.Start();
	current_level = "Tutorial.tmx";

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
	ingameTextWave = App->gui->CreateGuiElement(Types::text, 631, 0, { 0, 0, 49, 49 }, ingameTopBar, nullptr, "0", App->font->defaultfont);

	LoadTiledEntities();
	//if (ret) ret = CreateButtonsUI();
	

	return ret;
}

bool j1Scene::CreatePauseMenu() 
{


	if (pausemenuBackground == nullptr) {
		App->audio->PlayFx(-1, App->audio->pause_fx_out, 0);
		App->audio->MusicVolume(0.2f);
		pausemenuBackground = App->gui->CreateGuiElement(Types::image, 347, -342, { 2292, 731, 586, 483 }, ingameUI);
	}


	pausemenuButtonResume = App->gui->CreateGuiElement(Types::button, 150, 100, { 0, 63, 303, 42 }, pausemenuBackground, this, NULL);
	pausemenuButtonResume->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	pausemenuTextResume = App->gui->CreateGuiElement(Types::text, 85, 4, { 0, 0, 138, 30 }, pausemenuButtonResume, nullptr, "Resume");

	pausemenuButtonSave = App->gui->CreateGuiElement(Types::button, 150, 200, { 0, 63, 303, 42 }, pausemenuBackground, this, NULL);
	pausemenuButtonSave->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	pausemenuTextSave = App->gui->CreateGuiElement(Types::text, 115, 4, { 0, 0, 138, 30 }, pausemenuButtonSave, nullptr, "Save");

	pausemenuButtonLoad = App->gui->CreateGuiElement(Types::button, 150, 250, { 0, 63, 303, 42 }, pausemenuBackground, this, NULL);
	pausemenuButtonLoad->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	pausemenuTextLoad = App->gui->CreateGuiElement(Types::text, 115, 4, { 0, 0, 138, 30 }, pausemenuButtonLoad, nullptr, "Load");

	pausemenuButtonOptions = App->gui->CreateGuiElement(Types::button, 150, 300, { 0, 63, 303, 42 }, pausemenuBackground, this, NULL);
	pausemenuButtonOptions->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	pausemenuTextOptions = App->gui->CreateGuiElement(Types::text, 90, 4, { 0, 0, 138, 30 }, pausemenuButtonOptions, nullptr, "Options");

	pausemenuButtonExit = App->gui->CreateGuiElement(Types::button, 150, 350, { 0, 63, 303, 42 }, pausemenuBackground, this, NULL);
	pausemenuButtonExit->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	pausemenuTextExit = App->gui->CreateGuiElement(Types::text, 115, 4, { 0, 0, 138, 30 }, pausemenuButtonExit, nullptr, "Exit");

	App->entity->pause = true;

	return true;
}

bool j1Scene::DeletePauseMenu() {

	App->audio->PlayFx(-1, App->audio->pause_fx_in, 0);
	//App->audio->MusicVolume(1.0f);
	if (optionsMenu)
	{
		DeleteOptions();
	}
	else 
	{
		DeletePauseMenuButtons();
	}

	pausemenuBackground->to_delete = true;
	pausemenuBackground = nullptr;

	App->entity->pause = false;

	return true;
}

bool j1Scene::DeletePauseMenuButtons() 
{
	pausemenuButtonResume->to_delete = true;
	pausemenuTextResume->to_delete = true;
	pausemenuButtonSave->to_delete = true;
	pausemenuTextSave->to_delete = true;
	pausemenuButtonLoad->to_delete = true;
	pausemenuTextLoad->to_delete = true;
	pausemenuButtonOptions->to_delete = true;
	pausemenuTextOptions->to_delete = true;
	pausemenuButtonExit->to_delete = true;
	pausemenuTextExit->to_delete = true;

	pausemenuButtonResume = nullptr;
	pausemenuTextResume = nullptr;
	pausemenuButtonSave = nullptr;
	pausemenuTextSave = nullptr;
	pausemenuButtonLoad = nullptr;
	pausemenuTextLoad = nullptr;
	pausemenuButtonOptions = nullptr;
	pausemenuTextOptions = nullptr;
	pausemenuButtonExit = nullptr;
	pausemenuTextExit = nullptr;

	return true;
}

bool j1Scene::CreateOptions() 
{
	if (current_scene == scenes::menu) 
	{
		optionsBackground = App->gui->CreateGuiElement(Types::image, 880, 120, { 2292, 731, 586, 483 });

		optionsTitleText = App->gui->CreateGuiElement(Types::text, 70, 80, { 0, 0, 138, 30 }, optionsBackground, nullptr, "Options");

		optionsMusicText = App->gui->CreateGuiElement(Types::text, 65, 130, { 0, 0, 138, 30 }, optionsBackground, nullptr, "Music", App->font->smallfont);
		optionsMusicSlider = App->gui->CreateGuiElement(Types::slider, 65, 160, { 306, 177, 176, 9 }, optionsBackground, this);
		optionsMusicSlider->setSliderPos(App->audio->volumemusic);
		optionsFxText = App->gui->CreateGuiElement(Types::text, 65, 180, { 0, 0, 138, 30 }, optionsBackground, nullptr, "Fx", App->font->smallfont);
		optionsFxSlider = App->gui->CreateGuiElement(Types::slider, 65, 210, { 306, 177, 176, 9 }, optionsBackground, this);
		optionsFxSlider->setSliderPos(App->audio->volumefx);

		optionsButtonFullScreen = App->gui->CreateGuiElement(Types::button, 65, 250, { 0, 63, 303, 42 }, optionsBackground, this, NULL);
		optionsButtonFullScreen->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
		optionsTextFullScreen = App->gui->CreateGuiElement(Types::text, 50, 4, { 0, 0, 138, 30 }, optionsButtonFullScreen, nullptr, "Full Screen");

		optionsButtonClose = App->gui->CreateGuiElement(Types::button, 65, 330, { 0, 63, 303, 42 }, optionsBackground, this, NULL);
		optionsButtonClose->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
		optionsTextClose = App->gui->CreateGuiElement(Types::text, 110, 4, { 0, 0, 138, 30 }, optionsButtonClose, nullptr, "Close");
	}
	else if(pauseMenu)
	{
		//Deleting pause buttons
		DeletePauseMenuButtons();

		//Creating options
		optionsTitleText = App->gui->CreateGuiElement(Types::text, 225, 80, { 0, 0, 138, 30 }, pausemenuBackground, nullptr, "Options");

		optionsMusicText = App->gui->CreateGuiElement(Types::text, 200, 130, { 0, 0, 138, 30 }, pausemenuBackground, nullptr, "Music", App->font->smallfont);
		optionsMusicSlider = App->gui->CreateGuiElement(Types::slider, 200, 160, { 306, 177, 176, 9 }, pausemenuBackground, this);
		optionsMusicSlider->setSliderPos(App->audio->volumemusic);
		optionsFxText = App->gui->CreateGuiElement(Types::text, 200, 180, { 0, 0, 138, 30 }, pausemenuBackground, nullptr, "Fx", App->font->smallfont);
		optionsFxSlider = App->gui->CreateGuiElement(Types::slider, 200, 210, { 306, 177, 176, 9 }, pausemenuBackground, this);
		optionsFxSlider->setSliderPos(App->audio->volumefx);

		optionsButtonFullScreen = App->gui->CreateGuiElement(Types::button, 150, 250, { 0, 63, 303, 42 }, pausemenuBackground, this, NULL);
		optionsButtonFullScreen->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
		optionsTextFullScreen = App->gui->CreateGuiElement(Types::text, 50, 4, { 0, 0, 138, 30 }, optionsButtonFullScreen, nullptr, "Full Screen");

		optionsButtonClose = App->gui->CreateGuiElement(Types::button, 150, 300, { 0, 63, 303, 42 }, pausemenuBackground, this, NULL);
		optionsButtonClose->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
		optionsTextClose = App->gui->CreateGuiElement(Types::text, 105, 4, { 0, 0, 138, 30 }, optionsButtonClose, nullptr, "Close");
	}
	 
	return true;
}

bool j1Scene::DeleteOptions() 
{
	if (!pauseMenu) 
	{
		optionsBackground->to_delete = true;
	}
	else
	{
		CreatePauseMenu();
	}

	optionsTitleText->to_delete = true;
	optionsButtonFullScreen->to_delete = true;
	optionsTextFullScreen->to_delete = true;
	optionsButtonClose->to_delete = true;
	optionsTextClose->to_delete = true;
	optionsMusicText->to_delete = true;
	optionsMusicSlider->to_delete = true;
	optionsFxText->to_delete = true;
	optionsFxSlider->to_delete = true;

	optionsTitleText = nullptr;
	optionsButtonFullScreen = nullptr;
	optionsTextFullScreen = nullptr;
	optionsButtonClose = nullptr;
	optionsTextClose = nullptr;
	optionsMusicText = nullptr;
	optionsMusicSlider = nullptr;
	optionsFxText = nullptr;
	optionsFxSlider = nullptr;

	optionsBackground = nullptr;
	return true;
}

bool j1Scene::CreateInGame() 
{
	bool ret = true;

	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	current_level = "First level design.tmx";
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
	ingameTextWave = App->gui->CreateGuiElement(Types::text, 631, 0, { 0, 0, 49, 49 }, ingameTopBar, nullptr, "0", App->font->defaultfont);

	App->fowManager->CreateFoWMap(App->map->data.width, App->map->data.height);
	time_loaded = 0;
	upgrade_swordman = 0;
	upgrade_archer = 0;
	upgrade_knight = 0;

	if (!wants_to_load)
	{
		LoadTiledEntities();
		App->wave->CreateSpawnBuildings();

	}
	else {
		active = true;
		App->LoadGame();
		wants_to_load = false;
	}

	if(ret) ret = CreateButtonsUI();

	App->wave->wave_ended.Start();
	App->wave->wave_ongoing = false;
	App->wave->spawn_buildings = 3;
	App->wave->to_win = false;

	return ret;
}

bool j1Scene::CreateButtonsUI()
{
		townHallButton = App->gui->CreateGuiElement(Types::button, 1000, 80, { 306, 125, 58, 50 }, ingameUI, this, NULL);
		townHallButton->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
		townHallImage = App->gui->CreateGuiElement(Types::image, 6, 6, { 1092, 49, 46, 38 }, townHallButton, nullptr, NULL);

		townHallWoodCostImage = App->gui->CreateGuiElement(Types::image, 990, 150, { 832, 5, 85, 26 }, ingameUI, nullptr, NULL);
		townHallStoneCostImage = App->gui->CreateGuiElement(Types::image, 990, 180, { 974, 5, 85, 26 }, ingameUI, nullptr, NULL);
		townHallWoodCostText = App->gui->CreateGuiElement(Types::text, 30, 0, { 0, 0, 138, 30 }, townHallWoodCostImage, nullptr, "200", App->font->smallfont);
		townHallStoneCostText = App->gui->CreateGuiElement(Types::text, 30, 0, { 0, 0, 138, 30 }, townHallStoneCostImage, nullptr, "300", App->font->smallfont);
	return true;
}

bool j1Scene::DeleteButtonsUI()
{
	//there are crahses sometimes in this code xD
	if (townHallWoodCostImage != nullptr) 
	{
		townHallWoodCostImage->to_delete = true;
		townHallStoneCostImage->to_delete = true;
		townHallWoodCostText->to_delete = true;
		townHallStoneCostText->to_delete = true;
		townHallButton->to_delete = true;

		townHallWoodCostImage = nullptr;
		townHallStoneCostImage = nullptr;
		townHallWoodCostText = nullptr;
		townHallStoneCostText = nullptr;
		townHallButton = nullptr;
	}
	return true;
}

bool j1Scene::CreateLogo() {

	//video
	//App->audio->PlayFx(1, App->audio->intro_fx, 0);
	intro_video = App->video->Load("Assets/video/evangelion-opening.ogv", App->render->renderer);

	loop = true;
	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

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

	return true;
}

bool j1Scene::CreateVictory() {
	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	/*scale_victory = 0.0f;
	speed_victory = 0.005f;

	victoryLogo = App->tex->Load("Assets/textures/gui/VictorySheet.png");

	//Loading UI
	SDL_Rect rect = { 1280, 0, 1280, 720 };

	victoryBackground = App->gui->CreateGuiElement(Types::image, 0, 0, rect);	

	//App->render->Blit(victoryLogo, 0, 0);

	victoryButtonContinue = App->gui->CreateGuiElement(Types::button, 480, 550, { 0, 63, 303, 42 }, victoryBackground, this, NULL);
	victoryButtonContinue->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	victoryTextContinue = App->gui->CreateGuiElement(Types::text, 75, 4, { 0, 0, 138, 30 }, victoryButtonContinue, nullptr, "Continue");
	

	//uncomment that to use text and not button to continue
	//victoryTextClick = App->gui->CreateGuiElement(Types::text, 450, 520, { 0, 0, 138, 30 }, victoryBackground, nullptr, "Press X to continue..");*/

	//victory music
	App->audio->PlayMusic("Human/Human_Victory.ogg", 2.0F);


	return true;
}

bool j1Scene::CreateDefeat() {
	//Reseting camera to (0,0) position
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	/*scale_defeat = 0.0f;
	speed_defeat = 0.005f;

	defeatLogo = App->tex->Load("Assets/textures/gui/DefeatSheet.png");

	//Loading UI
	SDL_Rect rect = { 1280, 0, 1280, 720 };

	defeatBackground = App->gui->CreateGuiElement(Types::image, 0, 0, rect);

	//App->render->Blit(victoryLogo, 0, 0);

	defeatButtonContinue = App->gui->CreateGuiElement(Types::button, 480, 550, { 0, 63, 303, 42 }, defeatBackground, this, NULL);
	defeatButtonContinue->setRects({ 305, 63, 303, 42 }, { 0, 107, 303, 42 });
	defeatTextContinue = App->gui->CreateGuiElement(Types::text, 75, 4, { 0, 0, 138, 30 }, defeatButtonContinue, nullptr, "Continue");

	//uncomment that to use text and not button to continue
	//victoryTextClick = App->gui->CreateGuiElement(Types::text, 450, 520, { 0, 0, 138, 30 }, victoryBackground, nullptr, "Press X to continue..");*/

	//victory music
	App->audio->PlayMusic("Human/Human_Defeat.ogg", 2.0F);

	return true;
}

bool j1Scene::DeleteUI() 
{
	if (townHallButton != nullptr) DeleteButtonsUI();
	if (optionsBackground != nullptr) DeleteOptions();
	if (pauseMenu) DeletePauseMenu();

	App->gui->DeleteAllGui();

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
	ingameTextWave = nullptr;
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

	if (logoSheet != nullptr) App->tex->UnLoad(logoSheet);

	return true;
}


void j1Scene::GuiInput(GuiItem* guiElement) {
	if (UiEnabled)
	{
		//Menu buttons
		if (guiElement == menuButtonNewGame) {
			
			App->tutorial->ActualState = ST_Tutorial_Q0;
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			App->audio->PauseMusic(1.0f);
			App->fade->FadeToBlack(scenes::tutorial, 2.0f);
			//tutorial = true;
		}
		else if (guiElement == menuButtonExit) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			App->quitGame = true;
		}
		else if (guiElement == menuButtonLoadGame) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			if (App->CheckSaveGame()) {

				App->fade->FadeToBlack(scenes::ingame, 2.0f);
				wants_to_load = true;
				//App->LoadGame();

			}

		}
		else if (guiElement == menuButtonOptions) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			if (optionsMenu) DeleteOptions();
			else CreateOptions();
			optionsMenu = !optionsMenu;
		}

		//Options Button
		if (guiElement == optionsButtonClose) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			DeleteOptions();
			optionsMenu = false;
		}
		else if (guiElement == optionsMusicSlider) {
			App->audio->MusicVolume(optionsMusicSlider->returnSliderPos());
		}
		else if (guiElement == optionsFxSlider) {
			App->audio->FxVolume(-1, optionsFxSlider->returnSliderPos());
		}
		else if (guiElement == optionsButtonFullScreen) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);

			if (!fullscreen)
			{
				SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_FULLSCREEN);
				
	/*			if(current_scene == scenes::ingame || current_scene == scenes::tutorial)
					App->minimap->Start();*/
			}
			else
			{
				SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_SHOWN);
				//if (current_scene == scenes::ingame || current_scene == scenes::tutorial)
				//	//App->minimap->Start();

			}

			fullscreen = !fullscreen;
		}

		//InGame Buttons
		if (guiElement == ingameButtonMenu) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			if (!pauseMenu) CreatePauseMenu();
			else DeletePauseMenu();

			pauseMenu = !pauseMenu;
		}
		else if (guiElement == townHallButton) {
			App->audio->PlayFx(-1, App->audio->normal_click, 0);
			if (!Building_preview && !App->entity->IsSomethingSelected())
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, mouse_position.x, mouse_position.y);
				Building_preview = true;
			}
		}

		//Pause Menu Buttons
		if (guiElement == pausemenuButtonResume) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			pauseMenu = false;
			DeletePauseMenu();
		}
		else if (guiElement == pausemenuButtonSave) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			if(current_scene == scenes::ingame)
			App->SaveGame();

			//save
		}
		else if (guiElement == pausemenuButtonLoad) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			if (App->CheckSaveGame()) {

				App->fade->FadeToBlack(scenes::ingame, 2.0f);
				wants_to_load = true;
				pauseMenu = false;
				DeletePauseMenu();
				//App->LoadGame();

			}
			//load
		}
		else if (guiElement == pausemenuButtonOptions) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			//options
			CreateOptions();
			optionsMenu = true;
		}
		else if (guiElement == pausemenuButtonExit) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			DeletePauseMenu();
			pauseMenu = false;
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}

		/*//Victory Buttons
		if (guiElement == victoryButtonContinue) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}

		//Defeat Buttons
		if (guiElement == defeatButtonContinue) {
			App->audio->PlayFx(-1, App->audio->click_to_play, 0);
			App->fade->FadeToBlack(scenes::menu, 2.0f);
		}*/
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
		ingameTextGold->SetText(to_string(gold));
		break;
	case 2:
		wood += quantity;
		ingameTextWood->SetText(to_string(wood));
		break;
	case 3:
		stone += quantity;
		ingameTextStone->SetText(to_string(stone));
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

	ingameTextClock->SetText(str);
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

void j1Scene::UpdateCameraPosition(int speed) 
{

}
