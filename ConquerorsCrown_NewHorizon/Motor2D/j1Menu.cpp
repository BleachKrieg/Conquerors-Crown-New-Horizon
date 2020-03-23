#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Menu.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "EntityRequest.h"


j1Menu::j1Menu() : j1Module()
{
	name.create("menu");


}

// Destructor
j1Menu::~j1Menu()
{}

// Called before render is available
bool j1Menu::Awake(pugi::xml_node& config)
{
	LOG("Loading Menu");

	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Menu::Start()
{
	LOG("Start scene");

	debug = false;


	Hello = App->tex->Load("textures/Hello-World.png");
	debug_tex = App->tex->Load("textures/sprites/bullside.png");

	buttonNewGame = App->gui->CreateGuiElement(Types::button, 0, 0, { 444, 169, 244, 65 }, nullptr, this, NULL);
	buttonNewGame->setRects({ 444, 413, 244, 66 }, { 444, 661, 244, 65 });

	SDL_Rect rect = { 0, 0, 427, 218 };

	App->gui->CreateGuiElement(Types::text, 50, 14, rect, nullptr, this, "CONTINUE");
	App->gui->CreateGuiElement(Types::slider, 100, 100, { 28, 257, 12, 189 }, nullptr, this);
	App->gui->CreateGuiElement(Types::inputText, 400, 0, { 444, 661, 244, 65 }, nullptr, this);

	return true;
}

// Called each loop iteration
bool j1Menu::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Scene", Profiler::Color::Snow);


	return true;
}

// Called each loop iteration
bool j1Menu::Update(float dt)
{
	BROFILER_CATEGORY("Update_Scene", Profiler::Color::Tomato);

	//	App->render->Blit(Hello, 0, 0, NULL, 1.0f, 1.0f);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		App->render->camera.y += 500 * dt;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		App->render->camera.y -= 500 * dt;
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		if (debug)debug = false;
		else {
			debug = true;
		}

	int x, y;
	App->input->GetMousePosition(x, y);

	return true;
}

// Called each loop iteration
bool j1Menu::PostUpdate(float dt)
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1Menu::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
bool j1Menu::Load(pugi::xml_node & data)
{
	LOG("Loading saved game");



	return true;
}