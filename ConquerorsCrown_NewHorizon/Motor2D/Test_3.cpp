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

Test_3::Test_3(int posx, int posy) : StaticEnt(StaticEntType::TEST_3)
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
	canbuild = false;
	construction_time = 3;

	// Load all animations
	inconstruction.PushBack({ 399,410,96,81 }, 0.2, 0, 0, 0, 0);
	finishedconst.PushBack({ 403,273,96,95 }, 0.2, 0, 0, 0, 0);

	actualState = ST_BARRACK_PREVIEW;
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

	checkAnimation(dt);

	//Debug features
	if (App->scene->debug && actualState != ST_BARRACK_PREVIEW)
	{
		App->render->DrawCircle(position.x, position.y, vision, 0, 0, 200);
		App->render->DrawCircle(position.x, position.y, collrange, 200, 200, 0);
		App->render->DrawCircle(position.x, position.y, body, 0, 0, 200);
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
	if (actualState == ST_BARRACK_PREVIEW) {

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

bool Test_3::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool Test_3::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	if (actualState != ST_BARRACK_PREVIEW)
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
	}
	else
	{
		App->scene->Building_preview = false;
	}
	return true;
}

void Test_3::CheckWalkable(iPoint map)
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

void Test_3::checkAnimation(float dt)
{

	if (actualState == ST_BARRACK_PREVIEW)
	{
		current_animation = &finishedconst;

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && canbuild == true)
		{
			Mix_HaltChannel(-1);
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
					App->pathfinding->ChangeWalkability(tempPos, false);
				}
			}

			SpatialAudio(1, App->audio->construction, position.x, position.y);

			actualState = ST_BARRANCK_IN_CONSTRUCTION;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			Mix_HaltChannel(-1);
			SpatialAudio(2, App->audio->cancel_building, position.x, position.y);
			App->scene->Building_preview = false;
			to_delete = true;
		}

		GetTile();
		world.x += 32;
		world.y += 32;

		CheckWalkable(map);
	}

	if (actualState == ST_BARRANCK_IN_CONSTRUCTION)
	{
		current_animation = &inconstruction;

		if (timer.ReadSec() >= construction_time)
		{
			actualState = ST_BARRACK_FINISHED;
			Mix_HaltChannel(-1);
		}

	}

	if (actualState == ST_BARRACK_FINISHED)
	{
		// Finished Animation
		current_animation = &finishedconst;

		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 1.f, SpawnTypes::SWORDMAN, { (int)position.x + 7, (int)position.y + 30 });
			}
		}
	}
}