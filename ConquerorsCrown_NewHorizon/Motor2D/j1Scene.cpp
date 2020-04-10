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


j1Scene::j1Scene() : j1Module()
{
	name.create("scene");

	Building_preview = false;
	Building_preview = false;
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
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = false;

	LOG("Start scene");

	current_level = "First level design.tmx";
	debug = false;

	//Playing menu audio
	App->audio->PlayMusic("Audio/Music/Warcraft_II_Main_Menu.ogg", 1.0F);

	//debug_tex = App->tex->Load("textures/maps/Tile_select.png");
	//App->entity->CreateEntity(DynamicEnt::DynamicEntityType::TEST_1, 100, 200);

	if (CreateMenu())ret = true;

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
	case scenes::ingame:
		//Camera movement inputs
		int x, y;
		App->input->GetMousePosition(x, y);

		iPoint p = App->render->ScreenToWorld(x, y);

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

		//UI Position update
		ingameUIPosition = App->render->ScreenToWorld(0, 442);
		ingameUI->SetLocalPos(ingameUIPosition.x, ingameUIPosition.y);

		//Debug input
		if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) 
		{
			debug = !debug;
			App->map->blitColliders = !App->map->blitColliders;
		}
			

		//Temporal create entities inputs
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::SWORDMAN, { p.x, p.y });
		}
		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		{
			App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::ARCHER, { p.x, p.y });
		}
		if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && !Building_preview)
		{
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanBarracks, p.x, p.y);
			Building_preview = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && !Building_preview)
		{
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, p.x, p.y);
			Building_preview = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		{
			App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::GATHERER, { p.x, p.y });
		}
		if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		{
			App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::TROLL, { p.x, p.y });
		}
		//Draw the map
		App->map->Draw();

		break;
	}

	//App->render->Blit(debug_tex, p.x, p.y);

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	bool ret = true;
	
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

void j1Scene::ChangeScene(scenes next_scene) {
	//Deleting scene
	switch (current_scene)
	{
	case scenes::menu:
		DeleteUI();
		break;
	case scenes::ingame:
		DeleteUI();
		App->map->CleanUp();
		App->minimap->CleanUp();
		break;
	}
	//Creating scene
	switch (next_scene)
	{
	case scenes::menu:
		current_scene = scenes::menu;
		CreateMenu();
		App->audio->PlayMusic("Audio/Music/Warcraft_II_Main_Menu.ogg", 1.0F);
		break;
	case scenes::ingame:
		current_scene = scenes::ingame;
		CreateInGame();
		App->audio->PlayMusic("Audio/Music/Human/Human_Battle_1.ogg", 1.0F);
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

bool j1Scene::CreateInGame() {
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
		}
		RELEASE_ARRAY(data);
	}
	App->minimap->Start();
	/*SDL_SetRenderTarget(App->render->renderer, App->minimap->texture);
	App->minimap->CreateMinimap();
	SDL_SetRenderTarget(App->render->renderer, NULL);*/

	//Loading UI
	SDL_Rect downRect = { 0, 222, 1280, 278 };
	SDL_Rect topRect = { 0, 0, 1280, 50 };
	ingameUI = App->gui->CreateGuiElement(Types::image, 0, 442, downRect);
	ingameTopBar = App->gui->CreateGuiElement(Types::image, 0, -442, topRect, ingameUI);

	ingameButtonMenu = App->gui->CreateGuiElement(Types::button, 100, 4, { 0, 150, 138, 30 }, ingameTopBar, this, NULL);
	ingameButtonMenu->setRects({ 139, 150, 138, 30 }, { 0, 181, 138, 30 });
	ingameTextMenu = App->gui->CreateGuiElement(Types::text, 33, 4, { 0, 0, 138, 30 }, ingameButtonMenu, nullptr, "Menu", App->font->smallfont);

	return true;
}

bool j1Scene::DeleteUI() {
	
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
	App->gui->DeleteAllGui();
	return true;
}

void j1Scene::GuiInput(GuiItem* guiElement) {
	if (guiElement == menuButtonNewGame) {
		ChangeScene(scenes::ingame);
	}
	else if (guiElement == menuButtonExit) {
		App->quitGame = true;
	}
	else if (guiElement == ingameButtonMenu) {
		ChangeScene(scenes::menu);
	}
}
