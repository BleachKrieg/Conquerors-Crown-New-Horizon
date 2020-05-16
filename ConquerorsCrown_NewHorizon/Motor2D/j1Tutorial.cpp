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
#include "j1CutsceneManager.h"


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

	ActualState = ST_Tutorial_Q0;

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

	}

	LOG("%i, %i", App->render->camera.x, App->render->camera.y);


	return true;
}

void j1Tutorial::CheckTutorialStep(float dt)
{
	// Step 0
	if (ActualState == ST_Tutorial_Q0)
	{
		if (createUI)
		{
			createUI = false;

			App->cutscene->StartCutscene("Tutorial1");
		}

		if (App->render->camera.x == -428 && App->render->camera.y == -339)
		{
			createUI = true;
			ActualState = ST_Tutorial_Q1;
		}
	}

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
			
			CreatePopUpMessage(480, 96, "Uther", "Welcome to the Conquerors", "Crown new horizon tutorial!", "Here you'll learn the basics of", "the game and how to play!"," ");
		}
	}

	// Step 3
	if (ActualState == ST_Tutorial_Q3)
	{

		if (createUI)
		{
			createUI = false;

			CreatePopUpMessage(480, 105, "Uther", "In this game you have 3 types of", "resources: gold, wood and stone.", "With this materials you can", "construct buildings or recuit", "troops.");
			Arrow_1 = App->gui->CreateGuiElement(Types::image, 685, 40, { 2608, 212, 45, 64 }, App->scene->ingameTopBar);
			Arrow_2 = App->gui->CreateGuiElement(Types::image, 823, 40, { 2608, 212, 45, 64 }, App->scene->ingameTopBar);
			Arrow_3 = App->gui->CreateGuiElement(Types::image, 965, 40, { 2608, 212, 45, 64 }, App->scene->ingameTopBar);
		}
	}

	// Step 4
	if (ActualState == ST_Tutorial_Q4)
	{
		if (createUI)
		{
			createUI = false;

			// Create TownHall
			App->scene->active = true;
			App->entity->CreateStaticEntity(StaticEnt::StaticEntType::HumanTownHall, 820, 720);
			App->scene->active = false;

			CreatePopUpMessage(480, 96, "Uther", "Here's your Townhall, the", "most important building.", "Here you can recuit gatherers,", "units that are made to collect", "resources!");
		}
	}

	// Step 5
	if (ActualState == ST_Tutorial_Q5)
	{
		if (createUI)
		{
			createUI = false;

			App->scene->AddResource("wood", 100);
			App->scene->AddResource("gold", 100);
			CreatePopUpMessage(480, 96, "Uther", "We've added you some resources!", "Now, click the Townhall and", "create a gatherer clicking", "his icon.", " ");
			Arrow_4 = App->gui->CreateGuiElement(Types::image, 260, 150, { 2656, 212, 45, 64 }, App->scene->ingameTopBar);
		}
	}

	// Step 6
	if (ActualState == ST_Tutorial_Q6)
	{
		if (createUI)
		{
			createUI = false;
			CreatePopUpMessage(480, 96, "Uther", "Good job!", "In order to create buildings and", "units, you'll need to get more", "resources!", " ");
		}
	}

	// Step 7
	if (ActualState == ST_Tutorial_Q7)
	{

		if (createUI)
		{
			createUI = false;
			CreatePopUpMessage(480, 96, "Uther", "In order to send your gatherer to", "collect resources, you have to", "click on it and then click", "on the resource you want to", "collect.");
		}
	}

	// Step 8
	if (ActualState == ST_Tutorial_Q8)
	{
		if (createUI)
		{
			createUI = false;
			CreatePopUpMessage(480, 96, "Uther", "If you click on the trees, the", "gatherer will collect wood, on", "the rock for stone and on the", "mine for gold.", " ");
			Arrow_5 = App->gui->CreateGuiElement(Types::image, 685, 40, { 2608, 212, 45, 64 }, App->scene->ingameTopBar);
			Arrow_6 = App->gui->CreateGuiElement(Types::image, 823, 40, { 2608, 212, 45, 64 }, App->scene->ingameTopBar);
			Arrow_7 = App->gui->CreateGuiElement(Types::image, 965, 40, { 2608, 212, 45, 64 }, App->scene->ingameTopBar);
		}
	}

	// Step 9
	if (ActualState == ST_Tutorial_Q9)
	{
		if (createUI)
		{
			createUI = false;
			CreatePopUpMessage(480, 96, "Uther", "Knowing the basics, create a", "barrack fom the Townhall!", "Pro tip: You can create more", "gatherers to obtain the", "resources faster.");	
		}
	}

	// Step 10
	if (ActualState == ST_Tutorial_Q10)
	{

		if (createUI)
		{
			createUI = false;
			CreatePopUpMessage(480, 96, "Uther", "Well done!", "Now create a swordman to", "defend yourself from enemies!", "Pro tip: Upgrade the barrack", "to recuit different units.");
		}
	}

	// Step 11
	if (ActualState == ST_Tutorial_Q11)
	{
		MinimapActive = true;
		moveCamera = true;

		if (createUI)
		{
			createUI = false;
			CreatePopUpMessage(480, 96, "Gul'dan", "MUAHAHAHAHA", "I'LL DESTROY YOUR VILLAGE!", " ", " ", " ");
		}
	}

	if (ActualState == ST_Tutorial_Finished && App->scene->current_scene == scenes::tutorial)
	{
		App->scene->current_scene = scenes::ingame;
		App->fade->FadeToBlack(scenes::ingame, 2.0f);
	}
}

void j1Tutorial::CreatePopUpMessage(int x, int y, char* titletext, char* text1, char* text2, char* text3, char* text4, char* text5)
{
	PopUpImage = App->gui->CreateGuiElement(Types::image, x, y, { 2620, 0, 356, 209 }, App->scene->ingameTopBar);
	Uther_Image = App->gui->CreateGuiElement(Types::image, 818, 124, { 2573, 343, 497, 368 }, App->scene->ingameTopBar);
	PopUpTitleText = App->gui->CreateGuiElement(Types::text, x + 10, y + 10, { 0, 0, 138, 30 }, App->scene->ingameTopBar, nullptr, titletext, App->font->smallfont);
	PopUpText1 = App->gui->CreateGuiElement(Types::text, x + 10, y + 45, { 0, 0, 138, 30 }, App->scene->ingameTopBar, nullptr, text1, App->font->xs_font);
	PopUpText2 = App->gui->CreateGuiElement(Types::text, x + 10, y + 75, { 0, 0, 138, 30 }, App->scene->ingameTopBar, nullptr, text2, App->font->xs_font);
	PopUpText3 = App->gui->CreateGuiElement(Types::text, x + 10, y + 105, { 0, 0, 138, 30 }, App->scene->ingameTopBar, nullptr, text3, App->font->xs_font);
	PopUpText4 = App->gui->CreateGuiElement(Types::text, x + 10, y + 135, { 0, 0, 138, 30 }, App->scene->ingameTopBar, nullptr, text4, App->font->xs_font);
	PopUpText5 = App->gui->CreateGuiElement(Types::text, x + 10, y + 165, { 0, 0, 138, 30 }, App->scene->ingameTopBar, nullptr, text5, App->font->xs_font);

	if (ActualState != ST_Tutorial_Q5)
	{
		PopUpButton = App->gui->CreateGuiElement(Types::button, 600, -100, { 483, 126, 56, 48 }, App->scene->ingameUI, this, NULL);
		PopUpButton->setRects({ 541, 125, 58, 50 }, { 600, 125, 58, 50 });
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
	else if (guiElement == PopUpButton) {
		deleteUI(0);
		App->audio->PlayFx(-1, App->audio->normal_click, 0);

		if (ActualState == ST_Tutorial_Q2)
		{
			ActualState = ST_Tutorial_Q3;
		}
		else if (ActualState == ST_Tutorial_Q3)
		{
			ActualState = ST_Tutorial_Q4;
		}
		else if (ActualState == ST_Tutorial_Q4)
		{
			ActualState = ST_Tutorial_Q5;
		}
		else if (ActualState == ST_Tutorial_Q6)
		{
			ActualState = ST_Tutorial_Q7;
		}
		else if (ActualState == ST_Tutorial_Q7)
		{
			ActualState = ST_Tutorial_Q8;
		}
		else if (ActualState == ST_Tutorial_Q8)
		{
			ActualState = ST_Tutorial_Q9;
		}
		else if (ActualState == ST_Tutorial_Q9)
		{
			ActualState = ST_Tutorial_Q9_1;
		}
		else if (ActualState == ST_Tutorial_Q10)
		{
			ActualState = ST_Tutorial_Q10_1;
		}
		else if (ActualState == ST_Tutorial_Q11)
		{
			ActualState = ST_Tutorial_Q11_1;
		}
		else if (ActualState == ST_Tutorial_Q12)
		{
			ActualState = ST_Tutorial_Q13;
		}
		else if(ActualState == ST_Tutorial_Q10)
		{
			ActualState = ST_Tutorial_Finished;
		}
		createUI = true;
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

	//General delete
	if (step == 0)
	{
		PopUpImage->to_delete = true;
		Uther_Image->to_delete = true;
		PopUpTitleText->to_delete = true;
		PopUpText1->to_delete = true;
		PopUpText2->to_delete = true;
		PopUpText3->to_delete = true;
		PopUpText4->to_delete = true;
		PopUpText5->to_delete = true;

		if (PopUpButton != nullptr) 
		{
			PopUpButton->to_delete = true;
		}

		Uther_Image = nullptr;
		PopUpImage = nullptr;
		PopUpTitleText = nullptr;
		PopUpText1 = nullptr;
		PopUpText2 = nullptr;
		PopUpText3 = nullptr;
		PopUpText4 = nullptr;
		PopUpText5 = nullptr;
		PopUpButton = nullptr;

		if (ActualState == ST_Tutorial_Q3)
		{
			Arrow_1->to_delete = true;
			Arrow_1 = nullptr;
			Arrow_2->to_delete = true;
			Arrow_2 = nullptr;
			Arrow_3->to_delete = true;
			Arrow_3 = nullptr;
		}
		if (ActualState == ST_Tutorial_Q8)
		{
			Arrow_5->to_delete = true;
			Arrow_5 = nullptr;
			Arrow_6->to_delete = true;
			Arrow_6 = nullptr;
			Arrow_7->to_delete = true;
			Arrow_7 = nullptr;
		}
	}
	return true;
}