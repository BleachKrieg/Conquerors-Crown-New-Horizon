#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "HumanGatherer.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Input.h"
#include "J1GroupMov.h"
#include <math.h>

HumanGatherer::HumanGatherer(int posx, int posy) : DynamicEnt(DynamicEntityType::HUMAN_GATHERER)
{
	name.create("human_gatherer");

	// TODO: Should get all the DATA from a xml file
	work_state = WORK_STATE::NONE;
	speed = { NULL, NULL };
	life_points = 80;
	attack_vision = 200;
	attack_range = 0;
	time_attack = 0;
	attack_damage = 0;
	vision = 26;
	inv_size = 0;
	body = 13;
	chop_time = 0;
	position.x = posx;
	position.y = posy;
	orientation = SDL_FLIP_NONE;
	active = true;
	to_delete = false;
	isSelected = false;
	to_blit = true;
	selectable = true;
	following_target = false;
	can_attack = false;
	team = TeamType::PLAYER;
	target_entity = NULL;
	player_order = false;
	state = DynamicState::IDLE;
	entity_type = DynamicEntityType::HUMAN_GATHERER;
	work_name = "";

	// TODO ------------------------------------------
}

HumanGatherer::~HumanGatherer() {}

bool HumanGatherer::Start()
{
	bool found = false;
	for (int i = 0; i < App->entity->player_stat_ent.size() && !found; ++i)
	{
		if (App->entity->player_stat_ent[i]->name == "town_hall")
		{
			town_hall = App->entity->player_stat_ent[i];
			found = true;
		}
	}

	list<Animation*>::iterator animations_list;
	animations_list = App->entity->gatherer_animations.begin();
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

	current_animation = &moving_down;
	return true;
}

bool HumanGatherer::Update(float dt)
{
	BROFILER_CATEGORY("Update_HumanFootman", Profiler::Color::BlanchedAlmond);

	//Speed is resetted to 0 each iteration
	speed = { 0, 0 };
	origin = App->map->WorldToMap(position.x, position.y);

	if (App->scene->debug)
		life_points = 80;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;

	if (isSelected)
	{
		bool found = false;
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && App->input->screen_click)
		{
			work_state = WORK_STATE::NONE;
			target_entity = nullptr;
			iPoint pos;
			j1Entity* it;
			App->input->GetMousePosition(pos.x, pos.y);
			pos = App->render->ScreenToWorld(pos.x, pos.y);
			bool loop = true;
			SDL_Rect r;
			inv_size = 0;
			for (int i = 0; i < App->entity->resources_ent.size() && loop; ++i)
			{
				it = App->entity->resources_ent[i];
				r.x = it->position.x;
				r.y = it->position.y;
				r.w = 32;
				r.h = 32;
				if (pos.x > r.x && pos.x < (r.x + r.w) && pos.y > r.y && pos.y < (r.y + r.h))
				{
					work_space = it;
					loop = false;
					found = true;
					work_state = WORK_STATE::GO_TO_WORK;
					work_name = it->name;
					if (work_name == "mine")
						work_time = App->entity->mines_time;
					if (work_name == "quarry")
						work_time = App->entity->quarries_time;
					if (work_name == "tree")
						work_time = App->entity->trees_time;
				}
			}
			
			SpatialAudio(5, App->audio->go_gatherer, position.x, position.y);
		}
		if (!found && App->input->screen_click)
		{
			OrderPath(entity_type);
			inv_size = 0;
		}
	}

	
	if (work_state == WORK_STATE::GO_TO_WORK)
	{
		target_entity = work_space;
		if (position.DistanceTo(work_space->position) <= 100 && path.size() == 0)
		{
			path.clear();
			work_state = WORK_STATE::WORKING;
			target_entity = nullptr;
			if (work_name == "mine")
			{
				App->entity->lights = true;
				to_blit = false;
				isSelected = false;
			}
			start_time = timer.ReadMs();
		}
		player_order = true;
	}
	if (work_state == WORK_STATE::GO_TO_TOWNHALL)
	{
		if (position.DistanceTo(town_hall->position) <= 200 && path.size() == 0)
		{
			path.clear();
			work_state = WORK_STATE::GO_TO_WORK;
			target_entity = work_space;
			if (work_name == "mine")
				App->scene->AddResource("gold", inv_size);
			if (work_name == "quarry")
				App->scene->AddResource("stone", inv_size);
			if (work_name == "tree")
				App->scene->AddResource("wood", inv_size);
			player_order = true;
			inv_size = 0u;
			following_target = false;
		}
	}
	if (work_state == WORK_STATE::WORKING)
	{
		if (work_name == "mine") 
		{
			App->entity->lights = true;
			isSelected = false;
		}
		state = DynamicState::INTERACTING;
		if ((timer.ReadMs() - start_time) > work_time)
		{
			target_entity = town_hall;
			following_target = false;
			inv_size = 100;
			state = DynamicState::IDLE;
			work_state = WORK_STATE::GO_TO_TOWNHALL;
			to_blit = true;
			selectable = true;
		}
		else {
			if (chop_time >= 70 && work_name=="tree") {
				SpatialAudio(10, App->audio->wood_gatherer, position.x, position.y);
				//LOG("Position x: %i		Position y: %i", position.x, position.y);
				chop_time = 0;
			}

			chop_time++;
		}
	}

	GathererGoTos();

	Movement(dt);

	if (life_points <= 0)
		state = DynamicState::DYING;

	switch (state)
	{
	case DynamicState::IDLE:
		current_animation = &moving_right;
		current_animation->Reset();
	//	current_animation->loop = false;
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
		Death(entity_type);
		to_delete = true;
		break;
	}

	//App->render->DrawQuad({ (int)position.x, (int)position.y, 10, 10 }, 200, 200, 0);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	if (isSelected)
	App->render->Blit(App->entity->ally_sel_tex, (int)(position.x - 15), (int)(position.y) - 10);

	//	App->render->DrawCircle((int)position.x, (int)position.y, 20, 0, 200, 0, 200);
	if (to_blit)
		App->render->Blit(App->entity->gather_man_tex, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	return true;
}

bool HumanGatherer::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_HumanFootman", Profiler::Color::BurlyWood)
		
		return true;
}

bool HumanGatherer::CleanUp()
{
	close_entity_list.clear();
	colliding_entity_list.clear();
	path.clear();
	name.Clear();
	return true;
}