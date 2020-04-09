#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanTownHall.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"

HumanTownHall::HumanTownHall(int posx, int posy) : StaticEnt(StaticEntType::HumanTownHall)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 40;
	collrange = 25;
	selectable = false;
	isSelected = false;
	to_delete = false;
	canbuild = false;
	construction_time = 3;
	time_FX = 1;
	timer_queue = 0;
	// Load all animations
	inconstruction.PushBack({265,145,111,95}, 0.2, 0, 0, 0, 0);
	finishedconst2.PushBack({262,16,119,107}, 0.2, 0, 0, 0, 0);

	team = TeamType::NO_TYPE;
	actualState = ST_TOWNHALL_PREVIEW;
	life_points = 100;
}

HumanTownHall::~HumanTownHall()
{}

bool HumanTownHall::Start()
{

	return true;
}

bool HumanTownHall::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT)
	{
		life_points = 0;
	}
	if (life_points <= 0)
		to_delete = true;
	checkAnimation(dt);
	LOG("%d", life_points);
	//Debug features
	if (App->scene->debug && actualState != ST_TOWNHALL_PREVIEW)
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
	App->render->Blit(App->entity->building, world.x - 55, world.y - 55, r, 1.0f, 1.0f);

	//This render is placed behind the general blit for art purposes
	if (actualState == ST_TOWNHALL_PREVIEW) {

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

bool HumanTownHall::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool HumanTownHall::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	if (actualState != ST_TOWNHALL_PREVIEW)
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
			//	App->pathfinding->ChangeWalkability(tempPos, 1);
			}
		}
	}
	else
	{
		App->scene->Building_preview_TownHall = false;
	}
	return true;
}

void HumanTownHall::CheckWalkable(iPoint map)
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

void HumanTownHall::checkAnimation(float dt)
{

	if (actualState == ST_TOWNHALL_PREVIEW)
	{
		current_animation = &finishedconst2;

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && canbuild == true)
		{
			Mix_HaltChannel(-1);
			App->scene->Building_preview_TownHall = false;
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
				//	App->pathfinding->ChangeWalkability(tempPos, 2);
				}
			}

			SpatialAudio(1, App->audio->construction, position.x, position.y);

			actualState = ST_TOWNHALL_IN_CONSTRUCTION;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			Mix_HaltChannel(-1);
			SpatialAudio(2, App->audio->cancel_building, position.x, position.y);
			App->scene->Building_preview_TownHall = false;
			to_delete = true;
		}

		GetTile();
		world.x += 32;
		world.y += 32;

		CheckWalkable(map);
	}

	if (actualState == ST_TOWNHALL_IN_CONSTRUCTION)
	{
		current_animation = &inconstruction;
		team = TeamType::PLAYER;

		if (timer.ReadSec() >= construction_time)
		{
			Mix_HaltChannel(-1);
			actualState = ST_TOWNHALL_FINISHED;
			Mix_HaltChannel(-1);
		}
		else {
			if (timer.ReadSec() >= time_FX) {
				SpatialAudio(1, App->audio->construction, position.x, position.y);
				time_FX++;
			}
		}

	}

	if (actualState == ST_TOWNHALL_FINISHED)
	{
		// Finished Animation
		current_animation = &finishedconst2;

		CheckQueue();

		if (timer_queue < 0)
		{
			timer_queue = 0;
		}
		if (timer_queue > 0)
		{
			timer_queue = timer_queue - 1 * dt;
		}
		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 53, (int)position.y - 53, 105, 105 }, 200, 0, 0, 200, false);

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				timer_queue += 3;
				QueueTroop* item = new QueueTroop();
				item->time = timer_queue;
				Troop.push_back(item);
			}
		}
	}
}

void HumanTownHall::CheckQueue()
{
	for (int i = 0; i < Troop.size(); i++)
	{
		if (Troop[i]->timer.ReadSec() >= Troop[i]->time)
		{
			App->requests->AddRequest(Petition::SPAWN, 0, SpawnTypes::GATHERER, { (int)position.x + 7, (int)position.y + 30 });

			Troop.erase(Troop.begin() + i);
			i--;
		}
	}
}