#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "Grunt_Enemy.h"
#include "DynamicEnt.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "J1GroupMov.h"
#include "FoWManager.h"
#include <math.h>

GruntEnemy::GruntEnemy(int posx, int posy) : DynamicEnt(DynamicEntityType::ENEMY_GRUNT)
{
	name.create("enemy_grunt");


	// TODO: Should get all the DATA from a xml file
	speed = { NULL, NULL };
	life_points = 100;
	attack_vision = 200;
	attack_range = 30;
	time_attack = 1400;
	attack_damage = 12;
	vision = 26;
	body = 13;
	coll_range = 13;
	position.x = posx;
	position.y = posy;
	orientation = SDL_FLIP_NONE;
	to_delete = false;
	isSelected = false;
	active = true;
	selectable = false;
	following_target = false;
	player_order = false;
	can_attack = true;
	team = TeamType::IA;
	target_entity = nullptr;
	state = DynamicState::IDLE;
	entity_type = DynamicEntityType::ENEMY_GRUNT;


	// TODO ------------------------------------------
}

GruntEnemy::~GruntEnemy() {}

bool GruntEnemy::Start()
{
	list<Animation*>::iterator animations_list;
	animations_list = App->entity->grunt_animations.begin();
	moving_up = **animations_list;
	++animations_list;
	moving_diagonal_up = **animations_list;
	++animations_list;
	moving_right = **animations_list;
	++animations_list;
	moving_diagonal_down = **animations_list;
	++animations_list;
	moving_down = **animations_list;
	++animations_list;
	attacking_up = **animations_list;
	++animations_list;
	attacking_diagonal_up = **animations_list;
	++animations_list;
	attacking_right = **animations_list;
	++animations_list;
	attacking_diagonal_down = **animations_list;
	++animations_list;
	attacking_down = **animations_list;
	++animations_list;
	death_up = **animations_list;
	++animations_list;
	death_down = **animations_list;
	++animations_list;

	current_animation = &moving_down;

	spawn = nullptr;
	time = 10;
	followpath = 0;
	change_direction = true;
	return true;
}

bool GruntEnemy::Update(float dt)
{
	BROFILER_CATEGORY("Update_GruntEnemy", Profiler::Color::BlanchedAlmond);

	speed = { 0, 0 };
	origin = App->map->WorldToMap(position.x, position.y);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;


	AttackTarget(entity_type);

	Movement(dt);

	origin = App->map->WorldToMap(position.x, position.y);

	if (target_entity == nullptr)
	{
		if (spawn != nullptr)
		{
			if (path.empty() && time >= 5)
			{
				iPoint target = App->map->WorldToMap(spawn->targetpos.x, spawn->targetpos.y);
				App->pathfinding->RequestPath(origin, target, this);
				idletime.Start();
				followpath = 0;
				change_direction = true;
			}
		}

	}
	time = idletime.ReadSec();
	if (life_points <= 0)
		state = DynamicState::DYING;

	switch (state)
	{
	case DynamicState::IDLE:
		current_animation = &moving_right;
		current_animation->Reset();
		current_animation->loop = false;
		break;
	case DynamicState::UP:
		current_animation = &moving_up;
		break;
	case DynamicState::DOWN:
		current_animation = &moving_down;
		break;
	case DynamicState::HORIZONTAL:
		current_animation = &moving_right;
		break;
	case DynamicState::DIAGONAL_UP:
		current_animation = &moving_diagonal_up;
		break;
	case DynamicState::DIAGONAL_DOWN:
		current_animation = &moving_diagonal_down;
		break;
	case DynamicState::INTERACTING:
		current_animation = &attacking_right;
		break;
	case DynamicState::DYING:
		if (App->movement->ai_selected == this)
			App->movement->ai_selected = nullptr;
		Death(entity_type);
		break;
	}


	//App->render->DrawQuad({ (int)position.x, (int)position.y, 10, 10 }, 200, 200, 0);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	if (isSelected)
		App->render->Blit(App->entity->enemy_sel_tex, (int)(position.x - 20), (int)(position.y) - 10);

	/*	App->render->DrawCircle((int)position.x, (int)position.y, 20, 200, 0, 0, 200);*/
	if (isSelected && App->movement->ai_selected != this && App->movement->ai_selected != nullptr)
		isSelected = false;

	iPoint worldDrawPos, screenPos;
	worldDrawPos = { (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2) };
	iPoint mapPos =	App->map->WorldToMap((int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2));


	FoWDataStruct* tileInfo = App->fowManager->GetFoWTileState({ mapPos.x, mapPos.y });
	int fogId = -1;
	int shroudId = -1;

	if (tileInfo != nullptr)
	{

		if (App->fowManager->bitToTextureTable.find(tileInfo->tileFogBits) != App->fowManager->bitToTextureTable.end())
		{
			fogId = App->fowManager->bitToTextureTable[tileInfo->tileFogBits];
		}

		if (App->fowManager->bitToTextureTable.find(tileInfo->tileShroudBits) != App->fowManager->bitToTextureTable.end())
		{
			shroudId = App->fowManager->bitToTextureTable[tileInfo->tileShroudBits];
		}

	}
	if(fogId == -1 && shroudId == -1 || App->scene->debug)
	App->render->Blit(App->entity->grunt_tex, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	return true;
}

bool GruntEnemy::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_GruntEnemy", Profiler::Color::BurlyWood)

		return true;
}

bool GruntEnemy::CleanUp()
{
	close_entity_list.clear();
	colliding_entity_list.clear();
	path.clear();
	name.Clear();
	return true;
}