#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "DynamicEnt.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "J1GroupMov.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Pathfinding.h"

DynamicEnt::DynamicEnt(DynamicEntityType type) : j1Entity(entityType::DYNAMIC)
{
	death_counter = 0;
}

DynamicEnt::~DynamicEnt()
{}



bool DynamicEnt::Awake(pugi::xml_node& config)
{
	return true;
}

bool DynamicEnt::CleanUp()
{

	return true;
}

void DynamicEnt::CheckCollisions(fPoint* speed)
{
	iPoint coord;
	
	list<MapLayer*>::iterator Layer_list;
	MapLayer* layer;


	for (Layer_list = App->map->data.layers.begin(); Layer_list != App->map->data.layers.end(); ++Layer_list)
	{
		layer = *Layer_list;
		if (layer->returnPropValue("Navigation") == 1) {
			coord = App->map->WorldToMap((int)(position.x + speed->x), (int)position.y);
			if (layer->Get(coord.x, coord.y) != 0) speed->x = 0;

			coord = App->map->WorldToMap((int)position.x, (int)(position.y + speed->y));
			if (layer->Get(coord.x, coord.y) != 0) speed->y = 0;

		}
	}
}

void DynamicEnt::OrderPath(DynamicEntityType type)
{
	j1Entity* it;

	list<j1Entity*>::iterator selected_it;
	if (isSelected && entity_type != DynamicEntityType::HUMAN_GATHERER && App->movement->ai_selected != nullptr)
	{
		target_entity = App->movement->ai_selected;
	}
	else if (isSelected && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		App->input->GetMousePosition(mouse.x, mouse.y);
		mouse = App->render->ScreenToWorld(mouse.x, mouse.y);
		mouse = App->map->WorldToMap(mouse.x, mouse.y);
		relative_target = { 0,0 };

		for (selected_it = App->movement->selected.begin(); selected_it != App->movement->selected.end(); ++selected_it) {
			it = *selected_it;
			relative_target.x += it->position.x;
			relative_target.y += it->position.y;
		}
		relative_target.x = relative_target.x / App->movement->selected.size();
		relative_target.y = relative_target.y / App->movement->selected.size();
		fPoint distance_to_center{ position.x, position.y };

		relative_target.x = position.x - relative_target.x;
		relative_target.y = position.y - relative_target.y;
		relative_target = App->map->WorldToMap(relative_target.x, relative_target.y);
		relative_target += mouse;
		if ((mouse.x - relative_target.x) > 1)
		{
			relative_target.x = mouse.x - 1;
		}
		if ((mouse.x - relative_target.x) < -1)
		{
			relative_target.x = mouse.x + 1;
		}
		if ((mouse.y - relative_target.y) > 1)
		{
			relative_target.y = mouse.y - 1;
		}
		if ((mouse.y - relative_target.y) < -1)
		{
			relative_target.y = mouse.y + 1;
		}

		if (App->movement->selected.size() > 10)
		{
			if (App->pathfinding->RequestPath(origin, relative_target, this) == -1)
			{
				App->pathfinding->RequestPath(origin, mouse, this);
			}
		}
		else
		{
			App->pathfinding->RequestPath(origin, mouse, this);
		}

		player_order = true;
		followpath = 1;
		change_direction = true;
		if (entity_type == DynamicEntityType::HUMAN_FOOTMAN) {
			SpatialAudio(5, App->audio->go_footman, position.x, position.y);
		}
		if (entity_type == DynamicEntityType::HUMAN_ARCHER) {
			SpatialAudio(5, App->audio->go_archer, position.x, position.y);
		}
		if (entity_type == DynamicEntityType::HUMAN_GATHERER) {
			SpatialAudio(5, App->audio->go_gatherer, position.x, position.y);
		}

	}
}

void DynamicEnt::AttackTarget(DynamicEntityType type)
{
	//attack close enemy entity

	if (target_entity == NULL)
	{
		following_target = false;
	}
	if (target_entity != NULL)
	{
		/*uint distance = ((target_entity->position.x - position.x) * (target_entity->position.x - position.x)
			+ (target_entity->position.y - position.y) * (target_entity->position.y - position.y));*/

		int x = target_entity->position.x;
		int y = target_entity->position.y;

		float distance = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));

		//if (!following_target && distance > (attack_range * attack_range))
		if (!following_target && !player_order && distance >= attack_range + target_entity->body)
		{
			current_time = timer.ReadMs();
			following_target = true;
			iPoint targetPos = App->map->WorldToMap(target_entity->position.x, target_entity->position.y);
			App->pathfinding->RequestPath(origin, targetPos, this);
			followpath = 1;

		}

		// Finish attack

		if (distance < attack_range + target_entity->body)
		{
			state = DynamicState::INTERACTING;

			following_target = false;
			if (player_order == false)
			{
				path.clear();
				if ((timer.ReadMs() - current_time) >= time_attack)
				{
					target_entity->life_points -= attack_damage;
					current_time = timer.ReadMs();
					if (App->entity->max_audio_attacks < 1) {
						

						if (entity_type == DynamicEntityType::HUMAN_ARCHER) {
							SpatialAudio(2, App->audio->archer_attack, position.x, position.y);
							App->entity->max_audio_attacks++;

						}
						if (entity_type == DynamicEntityType::HUMAN_FOOTMAN) {
							SpatialAudio(3, App->audio->footman_attack, position.x, position.y);
							App->entity->max_audio_attacks++;

						}
						if (entity_type == DynamicEntityType::ENEMY_TROLL) {
							SpatialAudio(5, App->audio->troll_attack, position.x, position.y);
							App->entity->max_audio_attacks++;

						}
						if (entity_type == DynamicEntityType::ENEMY_GRUNT) {
							SpatialAudio(6, App->audio->grunt_attack, position.x, position.y);
							App->entity->max_audio_attacks++;

						}
						if (entity_type == DynamicEntityType::ENEMY_OGRE) {
							SpatialAudio(7, App->audio->ogre_attack, position.x, position.y);
							App->entity->max_audio_attacks++;

						}
					}
					
				}
			}
		}
		/*	else if(path.At(1) == NULL){

				target_entity = NULL;
				following_target = false;
			}*/

		if (target_entity->life_points <= 0)
		{
			target_entity = NULL;
			state = DynamicState::IDLE;
			current_time = timer.ReadMs();
			path.clear();
			following_target = false;
		}
	}
}
void DynamicEnt::GathererGoTos()
{
	if (target_entity == NULL)
	{
		following_target = false;
	}
	if (target_entity != NULL)
	{
		/*uint distance = ((target_entity->position.x - position.x) * (target_entity->position.x - position.x)
			+ (target_entity->position.y - position.y) * (target_entity->position.y - position.y));*/

		int x = target_entity->position.x;
		int y = target_entity->position.y;

		float distance = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));

		if (!following_target)
		{
			current_time = timer.ReadMs();
			following_target = true;
			iPoint targetPos = App->map->WorldToMap(target_entity->position.x, target_entity->position.y);
			App->pathfinding->RequestPath(origin, targetPos, this);
			followpath = 1;

		}
	}
}

void DynamicEnt::Movement(float dt)
{
	fPoint pathSpeed{ 0,0 };
	if (!path.empty())
	{
		if (followpath < path.size())
		{
			for (uint i = 0; i < path.size(); ++i)
			{
				iPoint nextPoint = App->map->MapToWorld(path.at(i).x, path.at(i).y);
				if (App->scene->debug)
				{
					if (i == followpath)
					{
						App->render->DrawQuad({ nextPoint.x + 14, nextPoint.y + 14, 12, 12 }, 200, 0, 0, 100);
					}
					else {
						App->render->DrawQuad({ nextPoint.x + 14, nextPoint.y + 14, 6, 6 }, 200, 0, 0, 100);

					}
				}
			}

			if (origin.x == path.at(followpath).x && origin.y == path.at(followpath).y)
			{
				followpath++;
				change_direction = true;
			}
			if (followpath < path.size())
			{

				if (path.at(followpath).x < origin.x) {
					pathSpeed.x = -1;
				}

				if (path.at(followpath).x > origin.x) {
					pathSpeed.x = +1;
				}

				if (path.at(followpath).y < origin.y) {
					pathSpeed.y = -1;
				}

				if (path.at(followpath).y > origin.y) {
					pathSpeed.y = 1;
				}
			}
		}
		else {
			following_target = false;
			player_order = false;
			path.clear();
			change_direction = true;
		}
	}
	if (pathSpeed.x != 0 && pathSpeed.y != 0)
	{
		pathSpeed.x /= 1.5;
		pathSpeed.y /= 1.5;
		
	}
	if (change_direction)
	{
		if (pathSpeed.x != 0)
		{
			state = DynamicState::HORIZONTAL;
			current_animation = &moving_right;

			if (pathSpeed.y < 0)
			{
				state = DynamicState::DIAGONAL_UP;
				current_animation = &moving_diagonal_up;
			}

			if (pathSpeed.y > 0)
			{
				state = DynamicState::DIAGONAL_DOWN;
				current_animation = &moving_diagonal_down;
			}
		}
		else if (pathSpeed.y != 0) {
			if (pathSpeed.y < 0)
			{
				state = DynamicState::UP;
				current_animation = &moving_up;
			}

			if (pathSpeed.y > 0)
			{
				state = DynamicState::DOWN;
				current_animation = &moving_down;
			}
		}
		else
		{
			state = DynamicState::IDLE;
		}
		if (pathSpeed.x < 0)
		{
			orientation = SDL_FLIP_HORIZONTAL;
		}
		if (pathSpeed.x > 0)
		{
			orientation = SDL_FLIP_NONE;
		}
		change_direction = false;

	}
	

		SaveNeighbours(&close_entity_list, &colliding_entity_list);

		fPoint separationSpeed;

		if (!colliding_entity_list.empty())
		{
			separationSpeed = App->movement->GetSeparationSpeed(colliding_entity_list, position);
		}
		else
		{
			separationSpeed.x = 0;
			separationSpeed.y = 0;
		}
	/*	if (App->scene->debug)
		{
			App->render->DrawCircle(position.x, position.y, vision, 0, 200, 0);
			App->render->DrawCircle(position.x, position.y, body, 0, 0, 200);
			App->render->DrawCircle(position.x, position.y, attack_vision, 200, 200, 0);
			App->render->DrawCircle(position.x, position.y, attack_range, 255, 0, 0);
		}*/
	
		

		fPoint cohesionSpeed;
		if (!close_entity_list.empty())
		{
			cohesionSpeed = App->movement->GetCohesionSpeed(close_entity_list, position, body);
		}
		else
		{
			cohesionSpeed.x = 0;
			cohesionSpeed.y = 0;
		}

		fPoint alignmentSpeed;
		if (!close_entity_list.empty())
		{
			alignmentSpeed = App->movement->GetDirectionSpeed(close_entity_list);
		}
		else
		{
			alignmentSpeed.x = 0;
			alignmentSpeed.y = 0;
		}

	
		if(team == TeamType::PLAYER)
			speed.x += 1.5 * pathSpeed.x + 1 * separationSpeed.x + 0.5 * cohesionSpeed.x + 0 * alignmentSpeed.x;
		else
	speed.x += 1 * pathSpeed.x + 1 * separationSpeed.x + 0.5 * cohesionSpeed.x + 0 * alignmentSpeed.x;
	
		speed.y += 1.5 * pathSpeed.y + 1 * separationSpeed.y + 0.5 * cohesionSpeed.y + 0 * alignmentSpeed.y;

	CheckCollisions(&speed);

	position.y += 60 * dt * speed.y;
	position.x += 60 * dt * speed.x;
}

void DynamicEnt::SaveNeighbours(list<j1Entity*>* close_entity_list, list<j1Entity*>* colliding_entity_list)
{
	j1Entity* it;

	colliding_entity_list->clear();
	close_entity_list->clear();

	uint closest_enemy = attack_vision;
	if (target_entity != NULL)
		closest_enemy = -1;

	for (int i = 0; i < App->entity->entities.size(); i++) {
		it = App->entity->entities[i];
		if (it != this)
		{
			int x = it->position.x;
			int y = it->position.y;

			float distance = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));
			if (distance < body + it->body)
			{
				colliding_entity_list->push_back(it);

			}
			if (can_attack && distance < attack_vision + it->body && team != it->team && it->team != TeamType::NO_TYPE && it->life_points > 0)
			{
				if (distance < closest_enemy)
				{
					closest_enemy = distance;
					target_entity = it;
				}
			}
			else {
				/*if (target_entity == it && !player_order)
				{
					target_entity = NULL;
				}*/
			}
			if(it->type == entityType::DYNAMIC)
			if (distance < vision + it->body)
			{
				close_entity_list->push_back(it);
			}
		}
	}
}

void DynamicEnt::Death(DynamicEntityType entity_type)
{
	current_animation = &death_down;
	death_counter++;
	if (death_counter == 2) {
		if (entity_type == DynamicEntityType::HUMAN_ARCHER) {
			SpatialAudio(8, App->audio->die_archer, position.x, position.y);

		}
		if (entity_type == DynamicEntityType::HUMAN_FOOTMAN) {
			SpatialAudio(8, App->audio->die_footman, position.x, position.y);
		}
		if (entity_type == DynamicEntityType::HUMAN_GATHERER) {
			SpatialAudio(8, App->audio->die_gatherer, position.x, position.y);
		}
		if (entity_type == DynamicEntityType::ENEMY_TROLL) {
			SpatialAudio(8, App->audio->die_troll, position.x, position.y);
		}	
		if (entity_type == DynamicEntityType::ENEMY_GRUNT) {
			SpatialAudio(8, App->audio->die_grunt, position.x, position.y);
		}
		if (entity_type == DynamicEntityType::ENEMY_OGRE) {
			SpatialAudio(8, App->audio->die_ogre, position.x, position.y);
		}
	}
//	LOG("DEATH: %i", death_counter);
	if (current_animation->Finished() == true) { to_delete = true; }
}