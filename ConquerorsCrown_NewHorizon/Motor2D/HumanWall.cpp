#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Audio.h"
#include "HumanWall.h"

Human_Wall::Human_Wall(int posx, int posy) : StaticEnt(StaticEntType::HumanWall)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 20;
	body = 15;
	coll_range = 25;
	active = true;
	selectable = true;
	isSelected = false;
	to_delete = false;
	canbuild = false;
	time_FX_barracks = 1;
	
	// Load all animations
	finishedconst.PushBack({ 711,570,32,32 }, 0.2, 0, 0, 0, 0);
	team = TeamType::PLAYER;
	actualState = ST_WALL_PREVIEW;
	life_points = 200;
}

Human_Wall::~Human_Wall()
{}

bool Human_Wall::Start()
{
	if (App->scene->active == true) {
		actualState = ST_WALL_AUTOMATIC;
	}

	deployed = false;
	return true;
}

bool Human_Wall::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	if (isSelected && App->movement->player_selected != this)
		isSelected = false;

	if (App->scene->debug)
		life_points = 200;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;

	if (life_points <= 0)
	{
		to_delete = true;
	}

	checkAnimation(dt);

	//Debug features
	if (App->scene->debug && actualState != ST_WALL_PREVIEW)
	{

	//	App->render->DrawQuad({ (int)position.x -16, (int)position.y - 16, 32, 32 }, 200, 0, 0, 200, false);

		iPoint pos = { (int)position.x, (int)position.y };
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint tempPos = pos;

		tempPos = App->map->MapToWorld(tempPos.x, tempPos.y);
	//	App->render->DrawQuad({ (int)(position.x - 16), (int)(position.y - 16), 32, 32 }, 200, 0, 0, 50);
	}

	//Final blit
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->building, world.x - 16, world.y - 16, r, 1.0f, 1.0f);

	//This render is placed behind the general blit for art purposes
	if (actualState == ST_WALL_PREVIEW) {

		if (canbuild)
		{
			App->render->DrawQuad({ world.x - 16, world.y - 16, 32, 32 }, 0, 200, 0, 100);
		}
		else
		{
			App->render->DrawQuad({ world.x - 16, world.y - 16, 32, 32 }, 200, 0, 0, 100);
		}
	}

	return true;
}

bool Human_Wall::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool Human_Wall::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	if (actualState != ST_WALL_PREVIEW)
	{
		iPoint pos = { (int)position.x, (int)position.y };
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint tempPos = pos;

		App->pathfinding->ChangeWalkability(tempPos, 1);
	}
	else
	{
		App->scene->Building_preview = false;
	}
	return true;
}

void Human_Wall::CheckWalkable(iPoint map)
{
	if (App->pathfinding->IsWalkable(map) == true)
	{
		canbuild = true;
	}
	else
	{
		canbuild = false;
	}
}

void Human_Wall::checkAnimation(float dt)
{
	if (actualState == ST_WALL_AUTOMATIC) {
		map = App->map->WorldToMap(position.x, position.y);
		App->scene->Building_preview = false;
		timer.Start();
		world.x = position.x;
		world.y = position.y;
		team = TeamType::PLAYER;

		iPoint pos = { (int)position.x, (int)position.y };
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint tempPos = pos;

		App->pathfinding->ChangeWalkability(tempPos, 2);

		actualState = ST_WALL_FINISHED;
	}

	if (actualState == ST_WALL_PREVIEW)
	{
		current_animation = &finishedconst;

		if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->input->screen_click && App->scene->stone >= 100 || (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && canbuild == true && App->scene->debug == true)
		{
			if (App->scene->debug == false)
			{
				App->scene->AddResource("stone", -100);
			}
			//Mix_HaltChannel(-1);
			
			App->scene->Building_preview = false;
			GetTile();
			world.x += 32;
			world.y += 32;
			position.x = world.x;
			position.y = world.y;


			iPoint pos = { (int)position.x, (int)position.y };
			pos = App->map->WorldToMap(pos.x, pos.y);
			iPoint tempPos = pos;

			App->pathfinding->ChangeWalkability(tempPos, 2);

			SpatialAudio(1, App->audio->construction, position.x, position.y);

			actualState = ST_WALL_FINISHED;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && App->input->screen_click)
		{
			SpatialAudio(1, App->audio->cancel_building, position.x, position.y);
			App->scene->Building_preview = false;
			team = TeamType::PLAYER;
			App->scene->wall_create = false;
			to_delete = true;
		}

		GetTile();
		world.x += 32;
		world.y += 32;

		CheckWalkable(map);
	}

	if (actualState == ST_WALL_FINISHED)
	{
		// Finished Animation
		current_animation = &finishedconst;

		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 18, (int)position.y - 18, 34, 34 }, 200, 0, 0, 200, false);
		}
	}
}