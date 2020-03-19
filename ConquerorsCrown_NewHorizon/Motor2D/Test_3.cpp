#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "Test_3.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"

Test_3::Test_3(int posx, int posy) : StaticEnt( StaticEntType::TEST_3)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 40;
	selectable = false;
	isSelected = false;
	to_delete = false;
	finished = false;
	construction_time = 3000;
	timer = 0;
	// Load all animations
}

Test_3::~Test_3()
{}

bool Test_3::Start()
{
	timer = SDL_GetTicks();

	return true;
}

bool Test_3::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	//App->render->Blit(App->entity->test_1_graphics, position.x + current_animation->pivotx[current_animation->returnCurrentFrame()], position.y + current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)), 1.0f);
	
	if (finished)
	{
		if (isSelected == true)
		{
			App->render->DrawCircle(position.x + 10, position.y + 10, 20, 200, 0, 0);

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 1.f, SpawnTypes::SWORDMAN, { (int)position.x + 7, (int)position.y + 30 });
			}
		}

		// Finished Animation
		App->render->DrawQuad({ (int)position.x, (int)position.y, 20, 20 }, 0, 0, 200);
	}
	else
	{
		// Construction Animation
		App->render->DrawQuad({ (int)position.x, (int)position.y, 20, 20 }, 200, 0, 0);
		
		if (SDL_GetTicks() >= construction_time + timer)
		{
			finished = true;
		}
	}


	if (App->scene->debug)
	{
		App->render->DrawCircle(position.x + 10, position.y + 10, vision, 0, 0, 200);
		App->render->DrawQuad({ (int)position.x - 3, (int)position.y - 3, 26, 26 }, 200, 0, 0, 200, false);
	}

	return true;
}

bool Test_3::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

	return true;
}

bool Test_3::CleanUp()
{

	return true;
}