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
#include "j1Tutorial.h"


j1Tutorial::j1Tutorial() : j1Module()
{
	name.create("tutorial");

}

// Destructor
j1Tutorial::~j1Tutorial()
{}

// Called before render is available
bool j1Tutorial::Awake(pugi::xml_node& config)
{
	bool ret = true;


	return ret;
}

// Called before the first frame
bool j1Tutorial::Start()
{
	bool ret = true;
	createUI = true;

	Button_Yes = nullptr;
	Button_Yes_Text = nullptr;
	Button_No = nullptr;
	Button_No_Text = nullptr;
	Question_1_text = nullptr;

	ActualState = ST_Tutorial_Q1;

	return ret;
}

// Called each loop iteration
bool j1Tutorial::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Scene", Profiler::Color::Snow);

	
	return true;
}

// Called each loop iteration
bool j1Tutorial::Update(float dt)
{
	BROFILER_CATEGORY("Update_Scene", Profiler::Color::Tomato);


	if (App->scene->tutorial) 
	{
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			App->scene->debug = !App->scene->debug;
			App->map->blitColliders = !App->map->blitColliders;
		}
		if (App->scene->debug)
		{
			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::SWORDMAN, { App->scene->mouse_position.x, App->scene->mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::ARCHER, { App->scene->mouse_position.x,App->scene->mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::GATHERER, { App->scene->mouse_position.x, App->scene->mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::TROLL, { App->scene->mouse_position.x, App->scene->mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::OGRE, { App->scene->mouse_position.x, App->scene->mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::GRUNT, { App->scene->mouse_position.x, App->scene->mouse_position.y });
			}
			if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN && !App->scene->Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanBarracks, App->scene->mouse_position.x, App->scene->mouse_position.y);
				App->scene->Building_preview = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN && !App->scene->Building_preview)
			{
				App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, App->scene->mouse_position.x, App->scene->mouse_position.y);
				App->scene->Building_preview = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
			{
				App->scene->AddResource("wood", 100);
				App->scene->AddResource("stone", +100);
				App->scene->AddResource("gold", +100);
			}
		}
		// Camera movement inputs
			int x, y;
		App->input->GetMousePosition(x, y);

		App->scene->mouse_position = App->render->ScreenToWorld(x, y);

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
		
		CheckTutorialStep(dt);

		if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		{
			ActualState = ST_Tutorial_Finished;
		}
	}

	/*LOG("%i, %i", App->render->camera.x, App->render->camera.y);*/

	return true;
}

void j1Tutorial::CheckTutorialStep(float dt)
{

	if (ActualState == ST_Tutorial_Q1)
	{
		if (createUI)
		{
			createUI = false;

			Button_Yes = App->gui->CreateGuiElement(Types::button, 400, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Yes->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Yes_Text = App->gui->CreateGuiElement(Types::text, 404, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "YES", App->font->smallfont);

			Button_No = App->gui->CreateGuiElement(Types::button, 800, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_No->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_No_Text = App->gui->CreateGuiElement(Types::text, 812, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "NO", App->font->smallfont);

			Question_1_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Greetings fellow stranger, do you want to skip the tutorial ?", App->font->defaultfont);
		}
	}

	
}
// Called each loop iteration
bool j1Tutorial::PostUpdate(float dt)
{
	bool ret = true;


	return ret;
}

// Called before quitting
bool j1Tutorial::CleanUp()
{
	
	return true;
}

void j1Tutorial::GuiInput(GuiItem* guiElement) {

	if (guiElement == Button_Yes)
	{

		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		App->fade->FadeToBlack(scenes::ingame, 2.0f);
		deleteUI();

	}
	else if (guiElement == Button_No)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);

	}
}

bool j1Tutorial::deleteUI() 
{
	if (Button_Yes != nullptr && Button_No != nullptr) 
	{
		Question_1_text->to_delete = true;
		Button_Yes->to_delete = true;
		Button_No->to_delete = true;
		Button_No_Text->to_delete = true; 
		Button_Yes_Text->to_delete = true;

		Button_Yes = nullptr;
		Button_No = nullptr;
		Question_1_text = nullptr;
		Button_No_Text = nullptr;
		Button_Yes_Text = nullptr;
	}

	return true;
}
