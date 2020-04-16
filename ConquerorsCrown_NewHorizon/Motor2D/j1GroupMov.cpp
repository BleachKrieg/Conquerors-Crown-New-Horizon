#include "J1EntityManager.h"
#include "p2Defs.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Map.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Entity.h"
#include <vector>

j1GroupMov::j1GroupMov() {
	name.create("entity");
}


j1GroupMov::~j1GroupMov() {

}

bool j1GroupMov::Awake(pugi::xml_node& config) {


	return true;
}

bool j1GroupMov::Start() {
	return true;

}
bool j1GroupMov::Update(float dt) {

	static iPoint origin, mouse;
	iPoint start;
	j1Entity* it;
	App->input->GetMousePosition(mouse.x, mouse.y);
	mouse = App->render->ScreenToWorld(mouse.x, mouse.y);
	// TODO 0 ---------------------- Nothing to do here, just getting you in context
	// Every time we press leftclick button, we create a rect. The we check all entities with
	// selectable bool activated. If selectable entity is inside the rectangle, we turn their
	// isSelected bool to true

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		App->render->DrawQuad({ origin.x, origin.y, mouse.x - origin.x, mouse.y - origin.y }, 0, 200, 0, 100, false);
		App->render->DrawQuad({ origin.x, origin.y, mouse.x - origin.x, mouse.y - origin.y }, 0, 200, 0, 50);
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		player_selected = nullptr;
		App->input->GetMousePosition(origin.x, origin.y);
		origin = App->render->ScreenToWorld(origin.x, origin.y);
		bool loop = true;
		SDL_Rect rect;
		for (int i = 0; i < App->entity->player_dyn_ent.size() && loop; ++i)
		{
			it = App->entity->player_dyn_ent[i];
			it->isSelected = false;
			rect = it->GetAnimation()->GetCurrentSize();
			if (origin.x > (it->position.x - rect.w/2) && origin.x < (it->position.x + rect.w/2) && origin.y > (it->position.y - rect.h/2) && origin.y < (it->position.y + rect.h/2))
			{
				it->isSelected = true;
				player_selected = it;
				loop = false;
			}
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		
		for (int i = 0; i < App->entity->entities.size(); i++) {
			it = App->entity->entities[i];

			/*if (it->selectable_buildings == true && mouse.x < origin.x + 10 && mouse.y < origin.y + 10 && mouse.x > origin.x - 10 && mouse.y > origin.y - 10)
			{
				int xstatic = it->position.x, ystatic = it->position.y;
				if (mouse.x > xstatic - 50 && mouse.x < xstatic + 50 && mouse.y > ystatic - 50 && mouse.y < ystatic + 50)
				{
					it->isSelected = true;
				}
				else {
					it->isSelected = false;
				}
			}*/
			if (it->selectable && it->type == j1Entity::entityType::DYNAMIC)
			{
				if(it != player_selected)
					it->isSelected = false;
				int x = it->position.x, y = it->position.y;
				// We store x and y positions
				// Now we check if it's inside the rect, so we can "select this entity"
				if (x > origin.x&& x < mouse.x) {
					if (y < origin.y && y > mouse.y)
					{
						it->isSelected = true;
					}
					else if (y > origin.y&& y < mouse.y)
					{
						it->isSelected = true;
					}
				}
				else if (x < origin.x && x > mouse.x) {
					if (y < origin.y && y > mouse.y)
					{
						it->isSelected = true;
					}
					else if (y > origin.y&& y < mouse.y)
					{
						it->isSelected = true;
					}
				}
				if (it->isSelected)
				{
					if (!NewGroup)
					{
						selected.clear();
					}
					NewGroup = true;
					selected.push_back(it);
				}
			}
		}

		if (NewGroup)
		{
			NewGroup = false;
		}

	}
	return true;
}
bool j1GroupMov::PostUpdate(float dt) {
	return true;

}
bool j1GroupMov::CleanUp() {
	return true;
}

fPoint j1GroupMov::GetSeparationSpeed(list<j1Entity*>colliding_entity_list, fPoint position) 
{
	// TODO 4 Iterate all neighbours and add their relative position vectors in a FPoint.
	// Then divide it by the neighbours number to obtain the average vector
	// Before normalizing the resulting vector, remember to invert it multiplying by -1.
	// Do not normalize it if the norm is 0, instead return speed = 0
	j1Entity* it;
	list<j1Entity*>::iterator neighbours_it;
	fPoint separationSpeed = { 0,0 };

	for (neighbours_it = colliding_entity_list.begin(); neighbours_it != colliding_entity_list.end(); ++neighbours_it) {
		it = *neighbours_it;
		separationSpeed.x +=  it->position.x - position.x;
		separationSpeed.y +=  it->position.y - position.y;
	}

		separationSpeed.x *= -1;
		separationSpeed.y *= -1;

		separationSpeed.x = separationSpeed.x / colliding_entity_list.size();
		separationSpeed.y = separationSpeed.y / colliding_entity_list.size();

		float norm = sqrt(pow((separationSpeed.x), 2) + pow((separationSpeed.y), 2));

		if (norm != 0)
		{
			separationSpeed.x = separationSpeed.x / norm;
			separationSpeed.y = separationSpeed.y / norm;
		}
		else 
		{
			separationSpeed.x = 0;
			separationSpeed.y = 0;
		}
	
	return separationSpeed;
}

fPoint j1GroupMov::GetCohesionSpeed(list<j1Entity*>close_entity_list, fPoint position, float body)
{
	// TODO 5 Pretty much like before, we iterate all close neighbours
	// But there's an addition. We need another fPoint, the MassCenter, which will initially use this 
	// entity position
	// To calculate it we add all the neighbours positions and divideto get the average
	// Now, we can get that cohesion speed, using mass center as reference, calculate a vector that will attract them
	// Don't forget to normalize it
	
	fPoint cohesionSpeed = { 0,0 };
	fPoint MassCenter{ position.x, position.y };
	j1Entity* it;
	list<j1Entity*>::iterator neighbours_it;

	for (neighbours_it = close_entity_list.begin(); neighbours_it != close_entity_list.end(); ++neighbours_it) {
		it = *neighbours_it;
		MassCenter.x += it->position.x;
		MassCenter.y += it->position.y;
	}

		MassCenter.x = MassCenter.x / (close_entity_list.size() + 1);
		MassCenter.y = MassCenter.y / (close_entity_list.size() + 1);

		cohesionSpeed.x = position.x - MassCenter.x;
		cohesionSpeed.y = position.y - MassCenter.y;

		float norm = sqrt(pow((cohesionSpeed.x), 2) + pow((cohesionSpeed.y), 2));

		if (norm != 0	&& cohesionSpeed.x > body + 0.5 || cohesionSpeed.x < -body - 0.5
						&& cohesionSpeed.y > body + 0.5 || cohesionSpeed.y < -body - 0.5)
		{
			cohesionSpeed.x = -1* cohesionSpeed.x / norm;
			cohesionSpeed.y = -1* cohesionSpeed.y / norm;
		}
		else
		{
			cohesionSpeed.x = 0;
			cohesionSpeed.y = 0;
		}
		/*if (cohesionSpeed.x < 14 && cohesionSpeed.x > -14)
		{
			cohesionSpeed.x = 0;
		}
		else
		{
			cohesionSpeed.x = -1 * cohesionSpeed.x / norm;
		}
		if (cohesionSpeed.y < 14 && cohesionSpeed.y > -14)
		{
			cohesionSpeed.y = 0;
		}
		else
		{
			cohesionSpeed.y = -1 * cohesionSpeed.y / norm;
		}*/
	
	return cohesionSpeed;
}

fPoint j1GroupMov::GetDirectionSpeed(list<j1Entity*>close_entity_list)
{
	//OPTIONAL TODO
	// We add the direction vectors of our neighbours, then divide it, and then normalize it
	// It's actually quite simple
	fPoint alignmentSpeed{ 0,0 };
	j1Entity* it;
	list<j1Entity*>::iterator neighbours_it;

	for (neighbours_it = close_entity_list.begin(); neighbours_it != close_entity_list.end(); ++neighbours_it) {

		it = *neighbours_it;

		alignmentSpeed.x += it->speed.x;

		alignmentSpeed.y += it->speed.y;
	}

		alignmentSpeed.x = alignmentSpeed.x / close_entity_list.size();

		alignmentSpeed.y = alignmentSpeed.y / close_entity_list.size();

	

		float norm = sqrt(pow((alignmentSpeed.x), 2) + pow((alignmentSpeed.y), 2));

		if (norm != 0)

		{
			alignmentSpeed.x = alignmentSpeed.x / norm;
			alignmentSpeed.y = alignmentSpeed.y / norm;
		}

		
	
	return alignmentSpeed;
}

