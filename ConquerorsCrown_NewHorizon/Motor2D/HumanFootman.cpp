#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "HumanFootman.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Input.h"
#include "J1GroupMov.h"
#include <math.h>
#include "FoWManager.h"


HumanFootman::HumanFootman(int posx, int posy) : DynamicEnt(DynamicEntityType::HUMAN_FOOTMAN)
{
	name.create("human_footman");

	// TODO: Should get all the DATA from a xml file
	speed = { NULL, NULL };
	life_points = 100;
	max_hp = life_points;
	attack_vision = 200;
	attack_range = 30;
	tier_swordman = 0;
	stats_upgrade_damage = 6;
	stats_upgrade_life = 50;
	time_attack = 1000;
	attack_damage = 12;
	vision = 26;
	body = 13;
	coll_range = 13;
	position.x = posx;
	position.y = posy;
	orientation = SDL_FLIP_NONE;
	to_delete = false;
	active = true;
	isSelected = false;
	selectable = true;
	following_target = false;
	player_order = false;
	can_attack = true;
	team = TeamType::PLAYER;
	target_entity = NULL;
	state = DynamicState::IDLE;
	entity_type = DynamicEntityType::HUMAN_FOOTMAN;

	visionEntity = App->fowManager->CreateFoWEntity({ posx, posy }, true);
	visionEntity->SetNewVisionRadius(5);

	speed_modifier = 1;

	// TODO ------------------------------------------
}

HumanFootman::~HumanFootman() {}

bool HumanFootman::Start()
{
	list<Animation*>::iterator animations_list;
	animations_list = App->entity->footman_animations.begin();
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

	

	return true;
}

bool HumanFootman::Update(float dt)
{
	BROFILER_CATEGORY("Update_HumanFootman", Profiler::Color::BlanchedAlmond);

	//Speed is resetted to 0 each iteration
	speed = { 0, 0 };
	origin = App->map->WorldToMap(position.x, position.y);

	if (App->scene->debug)
		life_points = max_hp;

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;
	
	OrderPath(entity_type);
	AttackTarget(entity_type);
	fPoint auxPos = position;
	Movement(dt);

	if (auxPos != position)
		visionEntity->SetNewPosition({ (int)position.x, (int)position.y });

	if (life_points <= 0)
		state = DynamicState::DYING;

	switch (state)
	{
	case DynamicState::IDLE:
		current_animation = &moving_right;
		current_animation->Reset();
		//current_animation->loop = false;
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
		if (target_entity != nullptr)
		{
			if (target_entity->position.x > position.x)
				orientation = SDL_FLIP_NONE;
			else
				orientation = SDL_FLIP_HORIZONTAL;
		}
		current_animation = &attacking_right;
		break;
	case DynamicState::DYING:
		Death(entity_type);
		break;
	}

	//App->render->DrawQuad({ (int)position.x, (int)position.y, 10, 10 }, 200, 200, 0);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	if (isSelected)
		App->render->Blit(App->entity->ally_sel_tex, (int)(position.x - 15), (int)(position.y)-10);
		//App->render->DrawCircle((int)position.x, (int)position.y, 20, 0, 200, 0, 200);
	
	if (tier_swordman != App->scene->upgrade_swordman)
	{
		tier_swordman = App->scene->upgrade_swordman;
		life_points += stats_upgrade_life;
		max_hp += stats_upgrade_life;
		attack_damage += stats_upgrade_damage;
		if (tier_swordman == 2)
		{
			life_points = 200;
			max_hp = 200;
			attack_damage = 24;
		}
	}
	
	if (attack_damage == 18)
	{
		App->render->Blit(App->entity->foot_man_tex2, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	}
	else if (attack_damage == 24)
	{
		App->render->Blit(App->entity->foot_man_tex3, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	}
	else 
	{ 
		App->render->Blit(App->entity->foot_man_tex, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation); 
	}

	hp_conversion = (float)25 / (float)max_hp;
	SDL_Rect section;
	section.x = 0;
	section.y = 0;
	section.w = ((int)life_points * hp_conversion);
	section.h = 2;
	if (life_points < max_hp)
	App->render->Blit(App->entity->life_bar, (int)(position.x - (*r).w / 4), (int)(position.y + (*r).h / 3), &section);

	return true;
}

bool HumanFootman::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_HumanFootman", Profiler::Color::BurlyWood)


		return true;
}

bool HumanFootman::CleanUp()
{
	close_entity_list.clear();
	colliding_entity_list.clear();
	visionEntity->deleteEntity = true;
	App->fowManager->foWMapNeedsRefresh = true;
	path.clear();
	name.Clear();
	return true;
}