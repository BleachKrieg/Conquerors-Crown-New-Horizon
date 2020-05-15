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
	MinimapActive = false;
	moveCamera = false;

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


	if (App->scene->current_scene == scenes::tutorial)
	{
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			App->scene->debug = !App->scene->debug;
			App->map->blitColliders = !App->map->blitColliders;
		}

		// Debug modes
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

		if (moveCamera)
		{
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


	return true;
}

void j1Tutorial::CheckTutorialStep(float dt)
{
	// Step 1
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

	// Step 2
	if (ActualState == ST_Tutorial_Q2)
	{
		if (createUI)
		{
			createUI = false;

			Question_2_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Welcome to the Conquerors Crown new horizon tutorial! ", App->font->defaultfont);
			Question_2_text_2 = App->gui->CreateGuiElement(Types::text, 115, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Here you'll learn the basics of the game and how to play!", App->font->defaultfont);
			Button_Next_2 = App->gui->CreateGuiElement(Types::button, 600, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Next_2->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Next_2_Text = App->gui->CreateGuiElement(Types::text, 614, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "-->", App->font->smallfont);
		}
	}

	// Step 3
	if (ActualState == ST_Tutorial_Q3)
	{

		if (createUI)
		{
			createUI = false;

			// Create TownHall
			App->scene->active = true;
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, 280, 270);
			App->scene->active = false;

			Question_3_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Here's your Townhall, the most important building", App->font->defaultfont);
			Question_3_text_2 = App->gui->CreateGuiElement(Types::text, 115, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Also, there's 3 types of resources: wood, stone and gold", App->font->defaultfont);
			Button_Next_3 = App->gui->CreateGuiElement(Types::button, 600, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Next_3->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Next_3_Text = App->gui->CreateGuiElement(Types::text, 614, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "-->", App->font->smallfont);
			
		}
	}

	// Step 4
	if (ActualState == ST_Tutorial_Q4)
	{
		if (createUI)
		{
			createUI = false;

			App->scene->AddResource("wood", 100);
			App->scene->AddResource("gold", 100);

			Question_4_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "We've added you some resources!", App->font->defaultfont);
			Question_4_text_2 = App->gui->CreateGuiElement(Types::text, 100, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Now click the Townhall and create a gatherer clicking on his icon", App->font->defaultfont);
		}
	}

	// Step 5
	if (ActualState == ST_Tutorial_Q5)
	{
		if (createUI)
		{
			createUI = false;

			Question_5_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Well done!", App->font->defaultfont);
			Question_5_text_2 = App->gui->CreateGuiElement(Types::text, 70, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "In order to create buildings and units, you'll need to get more resources", App->font->defaultfont);
			Button_Next_5 = App->gui->CreateGuiElement(Types::button, 600, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Next_5->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Next_5_Text = App->gui->CreateGuiElement(Types::text, 614, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "-->", App->font->smallfont);
		}
	}

	// Step 6
	if (ActualState == ST_Tutorial_Q6)
	{
		if (createUI)
		{
			createUI = false;

			Question_6_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "You can send your gatherer to collect resources!", App->font->defaultfont);
			Question_6_text_2 = App->gui->CreateGuiElement(Types::text, 70, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Click your gatherer and then click on the forest to collect wood", App->font->defaultfont);
			Button_Next_6 = App->gui->CreateGuiElement(Types::button, 600, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Next_6->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Next_6_Text = App->gui->CreateGuiElement(Types::text, 614, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "-->", App->font->smallfont);
		}
	}

	// Step 7
	if (ActualState == ST_Tutorial_Q7)
	{
		MinimapActive = true;
		moveCamera = true;

		if (createUI)
		{
			createUI = false;

			Question_7_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "The gatherer will automatically collect the resources and bring them back to the Townhall", App->font->defaultfont);
			Question_7_text_2 = App->gui->CreateGuiElement(Types::text, 70, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "You can click on the stone to collect it or the mine to collect gold", App->font->defaultfont);
			Button_Next_7 = App->gui->CreateGuiElement(Types::button, 600, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Next_7->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Next_7_Text = App->gui->CreateGuiElement(Types::text, 614, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "-->", App->font->smallfont);
		}
	}

	// Step 8
	if (ActualState == ST_Tutorial_Q8)
	{
		if (createUI)
		{
			createUI = false;

			Question_8_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Now that you know the basics, try to create a barrack!", App->font->defaultfont);
			Question_8_text_2 = App->gui->CreateGuiElement(Types::text, 70, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Pro tip: You can create more gatherers to collect more resources", App->font->defaultfont);
		}
	}

	// Step 9
	if (ActualState == ST_Tutorial_Q9)
	{
		if (createUI)
		{
			createUI = false;

			Question_9_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Oh no! There's an horde that wants to attack our village!", App->font->defaultfont);
			Question_9_text_2 = App->gui->CreateGuiElement(Types::text, 70, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Collect resources and recuit some swordman to attack them before they attack you!", App->font->defaultfont);
			Button_Next_9 = App->gui->CreateGuiElement(Types::button, 600, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Next_9->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Next_9_Text = App->gui->CreateGuiElement(Types::text, 614, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "-->", App->font->smallfont);
		
			// Spawn enemies
		}
	}

	// Step 10
	if (ActualState == ST_Tutorial_Q10)
	{
		if (createUI)
		{
			createUI = false;

			Question_10_text = App->gui->CreateGuiElement(Types::text, 115, -210, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Good job! You've defended the village!", App->font->defaultfont);
			Question_10_text_2 = App->gui->CreateGuiElement(Types::text, 70, -170, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "Now you can go to the real fight!", App->font->defaultfont);
			Button_Next_10 = App->gui->CreateGuiElement(Types::button, 600, -100, { 306, 125, 58, 50 }, App->scene->ingameUI, this, NULL);
			Button_Next_10->setRects({ 365, 125, 58, 50 }, { 424, 125, 58, 50 });
			Button_Next_10_Text = App->gui->CreateGuiElement(Types::text, 614, -85, { 0, 0, 138, 30 }, App->scene->ingameUI, nullptr, "-->", App->font->smallfont);
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

	// Step 1
	if (guiElement == Button_Yes)
	{

		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		App->fade->FadeToBlack(scenes::ingame, 2.0f);
		deleteUI(1);

	}
	else if (guiElement == Button_No)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(1);
		ActualState = ST_Tutorial_Q2;
	}

	// Step 2
	if (guiElement == Button_Next_2)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(2);
		ActualState = ST_Tutorial_Q3;
	}

	// Step 3
	if (guiElement == Button_Next_3)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(3);
		ActualState = ST_Tutorial_Q4;
	}

	// Step 4

	// Step 5
	if (guiElement == Button_Next_5)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(5);
		ActualState = ST_Tutorial_Q6;
	}

	// Step 6
	if (guiElement == Button_Next_6)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(6);
		ActualState = ST_Tutorial_Q7;
	}

	// Step 7
	if (guiElement == Button_Next_7)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(7);
		ActualState = ST_Tutorial_Q8;
	}

	// Step 8

	// Step 9
	if (guiElement == Button_Next_9)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(9);
		ActualState = ST_Tutorial_Q10;
	}

	// Step 10
	if (guiElement == Button_Next_10)
	{
		App->audio->PlayFx(-1, App->audio->normal_click, 0);
		deleteUI(10);
		App->fade->FadeToBlack(scenes::ingame, 2.0f);
	}
}

bool j1Tutorial::deleteUI(int step) 
{
	createUI = true;

	// Step 1
	if (step == 1)
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
	}

	// Step 2
	if (step == 2)
	{
		if (Button_Next_2 != nullptr)
		{
			Question_2_text->to_delete = true;
			Question_2_text_2->to_delete = true;
			Button_Next_2->to_delete = true;
			Button_Next_2_Text->to_delete = true;

			Button_Next_2 = nullptr;
			Button_Next_2_Text = nullptr;
			Question_2_text = nullptr;
			Question_2_text_2 = nullptr;
		}
	}

	// Step 3
	if (step == 3)
	{
		if (Button_Next_3 != nullptr)
		{
			Question_3_text->to_delete = true;
			Question_3_text_2->to_delete = true;
			Button_Next_3->to_delete = true;
			Button_Next_3_Text->to_delete = true;

			Button_Next_3 = nullptr;
			Button_Next_3_Text = nullptr;
			Question_3_text = nullptr;
			Question_3_text_2 = nullptr;
		}
	}

	// Step 4
	if (step == 4)
	{
		if (Question_4_text != nullptr)
		{
			Question_4_text->to_delete = true;
			Question_4_text_2->to_delete = true;

			Question_4_text = nullptr;
			Question_4_text_2 = nullptr;
		}
	}

	// Step 5
	if (step == 5)
	{
		if (Button_Next_5 != nullptr)
		{
			Question_5_text->to_delete = true;
			Question_5_text_2->to_delete = true;
			Button_Next_5->to_delete = true;
			Button_Next_5_Text->to_delete = true;

			Button_Next_5 = nullptr;
			Button_Next_5_Text = nullptr;
			Question_5_text = nullptr;
			Question_5_text_2 = nullptr;
		}
	}

	// Step 6
	if (step == 6)
	{
		if (Button_Next_6 != nullptr)
		{
			Question_6_text->to_delete = true;
			Question_6_text_2->to_delete = true;
			Button_Next_6->to_delete = true;
			Button_Next_6_Text->to_delete = true;

			Button_Next_6 = nullptr;
			Button_Next_6_Text = nullptr;
			Question_6_text = nullptr;
			Question_6_text_2 = nullptr;
		}
	}

	// Step 7
	if (step == 7)
	{
		if (Button_Next_7 != nullptr)
		{
			Question_7_text->to_delete = true;
			Question_7_text_2->to_delete = true;
			Button_Next_7->to_delete = true;
			Button_Next_7_Text->to_delete = true;

			Button_Next_7 = nullptr;
			Button_Next_7_Text = nullptr;
			Question_7_text = nullptr;
			Question_7_text_2 = nullptr;
		}
	}

	// Step 8
	if (step == 8)
	{
		if (Question_8_text != nullptr)
		{
			Question_8_text->to_delete = true;
			Question_8_text_2->to_delete = true;

			Question_8_text = nullptr;
			Question_8_text_2 = nullptr;
		}
	}

	// Step 9
	if (step == 9)
	{
		if (Question_9_text != nullptr)
		{
			Question_9_text->to_delete = true;
			Question_9_text_2->to_delete = true;
			Button_Next_9->to_delete = true;
			Button_Next_9_Text->to_delete = true;

			Button_Next_9 = nullptr;
			Button_Next_9_Text = nullptr;
			Question_9_text = nullptr;
			Question_9_text_2 = nullptr;
		}
	}

	// Step 10
	if (step == 10)
	{
		if (Question_10_text != nullptr)
		{
			Question_10_text->to_delete = true;
			Question_10_text_2->to_delete = true;
			Button_Next_10->to_delete = true;
			Button_Next_10_Text->to_delete = true;

			Button_Next_10 = nullptr;
			Button_Next_10_Text = nullptr;
			Question_10_text = nullptr;
			Question_10_text_2 = nullptr;
		}
	}

	return true;
}