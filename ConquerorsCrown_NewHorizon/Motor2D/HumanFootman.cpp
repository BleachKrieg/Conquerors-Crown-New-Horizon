#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanFootman.h"
#include "DynamicEnt.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Input.h"
#include "J1GroupMov.h"
#include <math.h>

HumanFootman::HumanFootman(int posx, int posy) : DynamicEnt(DynamicEntityType::HUMAN_FOOTMAN)
{
	name.create("human_footman");

	// TODO: Should get all the DATA from a xml file
	speed = { NULL, NULL };
	life_points = 100;
	vision = 26;
	body = 13;
	collrange = 16;
	position.x = posx;
	position.y = posy;
	orientation = SDL_FLIP_NONE;
	to_delete = false;
	isSelected = false;
	selectable = true;
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

	current_animation = &moving_down;
	return true;
}

bool HumanFootman::Update(float dt)
{
	BROFILER_CATEGORY("Update_HumanFootman", Profiler::Color::BlanchedAlmond);

	//Speed is resetted to 0 each iteration
	speed = { 0, 0 };
	origin = App->map->WorldToMap(position.x, position.y);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT)
		to_delete = true;

	/*j1Entity* it;

	list<j1Entity*>::iterator selected_it;


	if (isSelected && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		App->input->GetMousePosition(mouse.x, mouse.y);
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
			if (App->pathfinding->CreatePath(origin, relative_target) == -1)
			{
				App->pathfinding->CreatePath(origin, mouse);
			}
		}
		else
		{
			App->pathfinding->CreatePath(origin, mouse);
		}

		const p2DynArray<iPoint>* last_path = App->pathfinding->GetLastPath();
		path.Clear();
		for (uint i = 0; i < last_path->Count(); ++i)
		{
			path.PushBack({ last_path->At(i)->x, last_path->At(i)->y });
		}
		followpath = 1;
	}


	fPoint pathSpeed{ 0,0 };
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
		if (path.At(followpath)->x < origin.x && origin.y == path.At(followpath)->y) {
			pathSpeed.x = -1;
			current_animation = &moving_right;
			orientation = SDL_FLIP_HORIZONTAL;
		}

		if (path.At(followpath)->x > origin.x&& origin.y == path.At(followpath)->y) {
			pathSpeed.x = +1;
			current_animation = &moving_right;
			orientation = SDL_FLIP_NONE;
		}

		if (path.At(followpath)->y < origin.y && origin.x == path.At(followpath)->x) {
			pathSpeed.y = -1;
			current_animation = &moving_up;
		}

		if (path.At(followpath)->y > origin.y&& origin.x == path.At(followpath)->x) {
			pathSpeed.y = 1;
			current_animation = &moving_down;
		}

		if (path.At(followpath)->y < origin.y && path.At(followpath)->x < origin.x) {
			pathSpeed.y = -1;
			current_animation = &moving_diagonal_up;
		}

		if (path.At(followpath)->y > origin.y&& path.At(followpath)->x < origin.x) {
			pathSpeed.y = 1;
			current_animation = &moving_diagonal_down;
		}

		if (path.At(followpath)->y < origin.y && path.At(followpath)->x > origin.x) {
			pathSpeed.y = -1;
			current_animation = &moving_diagonal_up;
		}

		if (path.At(followpath)->y > origin.y&& path.At(followpath)->x > origin.x) {
			pathSpeed.y = 1;
			current_animation = &moving_diagonal_down;
		}

		if (origin.x == path.At(followpath)->x && origin.y == path.At(followpath)->y)
		{
			followpath++;
		}


	}


	list<j1Entity*>::iterator neighbours_it;

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
		App->render->DrawCircle(position.x + 5, position.y + 5, vision, 200, 0, 0);
		App->render->DrawCircle(position.x + 5, position.y + 5, collrange, 200, 200, 0);
		App->render->DrawCircle(position.x + 5, position.y + 5, body, 0, 0, 200);
	}

	fPoint cohesionSpeed;
	if (!close_entity_list.empty())
	{
		cohesionSpeed = App->movement->GetCohesionSpeed(close_entity_list, position);
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

	speed.x += 1.5 * pathSpeed.x + 1 * separationSpeed.x + 0.5 * cohesionSpeed.x + 0 * alignmentSpeed.x;
	speed.y += 1.5 * pathSpeed.y + 1 * separationSpeed.y + 0.5 * cohesionSpeed.y + 0 * alignmentSpeed.y;

	CheckCollisions(&speed);

	position.y += speed.y;
	position.x += speed.x;
*/	

	Movement();

	if (isSelected)
		App->render->DrawCircle((int)position.x + 5, (int)position.y + 5, 10, 0, 200, 0, 200);

	//App->render->DrawQuad({ (int)position.x, (int)position.y, 10, 10 }, 200, 200, 0);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->foot_man_tex, (int)position.x - 20, (int)position.y - 20, r, 1.0f, 1.0f, orientation);
	return true;
}

bool HumanFootman::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Archer_PostUpdate", Profiler::Color::BurlyWood)

		return true;
}

bool HumanFootman::CleanUp()
{
	close_entity_list.clear();
	colliding_entity_list.clear();
	path.Clear();
	name.Clear();
	return true;
}

//void HumanFootman::SaveNeighbours(list<j1Entity*>* close_entity_list, list<j1Entity*>* colliding_entity_list)
//{
//	list<j1Entity*>::iterator entities_list;
//	j1Entity* it;
//	colliding_entity_list->clear();
//	close_entity_list->clear();
//
//	for (entities_list = App->entity->entities.begin(); entities_list != App->entity->entities.end(); ++entities_list) {
//		it = *entities_list;
//		if (it != this && it->selectable)
//		{
//			int x = it->position.x;
//			int y = it->position.y;
//
//			float distance = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));
//			if (distance < collrange + it->body)
//			{
//				colliding_entity_list->push_back(it);
//
//			}
//			if (distance < vision + it->body)
//			{
//				close_entity_list->push_back(it);
//			}
//		}
//	}
//}