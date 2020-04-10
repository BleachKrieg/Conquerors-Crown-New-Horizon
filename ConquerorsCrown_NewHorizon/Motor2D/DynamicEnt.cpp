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
	time_FX_troops = 0.5;
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

void DynamicEnt::Movement()
{
	j1Entity* it;

	list<j1Entity*>::iterator selected_it;

	if (isSelected && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		timer2.Start();
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

		SpatialAudio(3, App->audio->walking, position.x, position.y);
	}

	

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
		if (!following_target && !player_order && distance > attack_range + target_entity->body)
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
			following_target = false;
			if (player_order == false)
			{
				path.Clear();
				if ((timer.ReadMs() - current_time) >= time_attack)
				{
					target_entity->life_points -= attack_damage;
					current_time = timer.ReadMs();
				}
			}
		}

		if (target_entity->life_points <= 0)
		{
			target_entity = NULL;
			current_time = timer.ReadMs();
			path.Clear();
		}
	}

	fPoint pathSpeed{ 0,0 };
	if (path.At(1) != NULL)
	{
		if (path.At(followpath) != NULL)
		{
			for (uint i = 0; i < path.Count(); ++i)
			{
				iPoint nextPoint = App->map->MapToWorld(path.At(i)->x, path.At(i)->y);
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

			if (origin.x == path.At(followpath)->x && origin.y == path.At(followpath)->y)
			{
				followpath++;
				change_direction = true;
			}
			if (path.At(followpath) != NULL)
			{

				if (path.At(followpath)->x < origin.x) {
					pathSpeed.x = -1;
				}

				if (path.At(followpath)->x > origin.x) {
					pathSpeed.x = +1;
				}

				if (path.At(followpath)->y < origin.y) {
					pathSpeed.y = -1;
				}

				if (path.At(followpath)->y > origin.y) {
					pathSpeed.y = 1;
				}

				if (timer2.ReadSec() >= time_FX_troops) {
				SpatialAudio(3, App->audio->walking, position.x, position.y);
				time_FX_troops += 0.5;
				LOG("Troop time: %.2f", time_FX_troops);
				}
			}
		}
		else {
			following_target = false;
			player_order = false;
			path.Clear();
			time_FX_troops = 0.5;
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
			current_animation = &moving_right;

			if (pathSpeed.y < 0)
			{
				current_animation = &moving_diagonal_up;
			}

			if (pathSpeed.y > 0)
			{
				current_animation = &moving_diagonal_down;
			}
		}
		else if (pathSpeed.y != 0) {
			if (pathSpeed.y < 0)
			{
				current_animation = &moving_up;
			}

			if (pathSpeed.y > 0)
			{
				current_animation = &moving_down;
			}
		}
		else
		{
			Mix_HaltChannel(-1);
			//idle anim
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
		if (App->scene->debug)
		{
			App->render->DrawCircle(position.x, position.y, vision, 0, 200, 0);
			App->render->DrawCircle(position.x, position.y, body, 0, 0, 200);
			App->render->DrawCircle(position.x, position.y, attack_vision, 200, 200, 0);
			App->render->DrawCircle(position.x, position.y, attack_range, 255, 0, 0);
		}
		if (isSelected)
			App->render->DrawCircle((int)position.x, (int)position.y, 20, 0, 200, 0, 200);


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

	position.y += speed.y;
	position.x += speed.x;
}

void DynamicEnt::SaveNeighbours(list<j1Entity*>* close_entity_list, list<j1Entity*>* colliding_entity_list)
{
	list<j1Entity*>::iterator entities_list;
	j1Entity* it;
	colliding_entity_list->clear();
	close_entity_list->clear();

	uint closest_enemy = attack_vision;
	if (target_entity != NULL)
		closest_enemy = -1;

	for (entities_list = App->entity->entities.begin(); entities_list != App->entity->entities.end(); ++entities_list) {
		it = *entities_list;
		if (it != this)
		{
			int x = it->position.x;
			int y = it->position.y;

			float distance = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));
			if (distance < body + it->body)
			{
				colliding_entity_list->push_back(it);

			}
			if (can_attack && distance < attack_vision + it->body && team != it->team && it->team != TeamType::NO_TYPE)
			{
				if (distance < closest_enemy)
				{
					closest_enemy = distance;
					target_entity = it;
				}
			}
			else {
				if (target_entity == it)
				{
					target_entity = NULL;
				}
			}
			if(it->type == entityType::DYNAMIC)
			if (distance < vision + it->body)
			{
				close_entity_list->push_back(it);
			}
		}
	}
}