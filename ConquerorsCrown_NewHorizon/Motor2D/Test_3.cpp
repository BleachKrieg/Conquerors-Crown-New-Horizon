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
#include "j1Pathfinding.h"

Test_3::Test_3(int posx, int posy) : StaticEnt( StaticEntType::TEST_3)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 45;
	collrange = 25;
	selectable = false;
	isSelected = false;
	to_delete = false;
	finished = false;
	preview = true;
	construction_time = 3;

	// Load all animations
	inconstruction.PushBack({ 399,410,96,81 }, 0.2, 0, 0, 0, 0);
	finishedconst.PushBack({ 403,273,96,95 }, 0.2, 0, 0, 0, 0);
}

Test_3::~Test_3()
{}

bool Test_3::Start()
{

	return true;
}

bool Test_3::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT)
		to_delete = true;

	//App->render->Blit(App->entity->test_1_graphics, position.x + current_animation->pivotx[current_animation->returnCurrentFrame()], position.y + current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)), 1.0f);
	
	if (finished)
	{
		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 1.f, SpawnTypes::SWORDMAN, { (int)position.x + 7, (int)position.y + 30 });
			}
		}

		// Finished Animation
		current_animation = &finishedconst;
	}
	else if(!finished && !preview)
	{
		// Construction Animation
		current_animation = &inconstruction;
		
		if (timer.ReadSec() >= construction_time)
		{
			finished = true;
		}
	}
	else if (preview)
	{
		current_animation = &finishedconst;

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			timer.Start();
			GetTile();
			position.x = p.x;
			position.y = p.y;

			iPoint pos = { (int)position.x, (int)position.y };
			pos = App->map->WorldToMap(pos.x, pos.y);
			iPoint tempPos = pos;

			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					tempPos.x = pos.x + i;
					tempPos.y = pos.y + j;
					App->pathfinding->ChangeWalkability(tempPos, false);
				}
			}
		
			preview = false;
		}
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			to_delete = true;
		}
	}


	if (App->scene->debug)
	{
		App->render->DrawCircle(position.x , position.y, vision, 0, 0, 200);
		App->render->DrawCircle(position.x , position.y, collrange, 200, 200, 0);
		App->render->DrawCircle(position.x , position.y, body, 0, 0, 200);
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

	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);

	// This is for the preview option
	if (!preview)
	{
		App->render->Blit(App->entity->building, p.x-50, p.y-50, r, 1.0f, 1.0f);
	}
	else
	{
		GetTile();

		App->render->Blit(App->entity->building, p.x - 50, p.y - 50, r, 1.0f, 1.0f);
		App->render->DrawQuad({ p.x - 50, p.y - 50, 96, 95 }, 0, 200, 0, 100);
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
	iPoint pos = { (int)position.x, (int)position.y };
	pos = App->map->WorldToMap(pos.x, pos.y);
	iPoint tempPos = pos;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			tempPos.x = pos.x + i;
			tempPos.y = pos.y + j;
			App->pathfinding->ChangeWalkability(tempPos, true);
		}
	}
	return true;
}