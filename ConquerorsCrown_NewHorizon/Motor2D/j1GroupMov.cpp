#include "J1EntityManager.h"
#include "p2Defs.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Map.h"
#include "DynamicEnt.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Entity.h"
#include <vector>

j1GroupMov::j1GroupMov() {
	name.create("group_movement");
}


j1GroupMov::~j1GroupMov() {

}

bool j1GroupMov::Awake(pugi::xml_node& config) {


	return true;
}

bool j1GroupMov::Start() {

	portrait_entity = nullptr;
	player_selected = nullptr;
	ai_selected = nullptr;

	return true;
}
bool j1GroupMov::Update(float dt) {

	static iPoint origin, mouse;
	iPoint start;
	j1Entity* it;
	if (ai_selected != nullptr)
	{
		ai_selected = nullptr;
	}
	App->input->GetMousePosition(mouse.x, mouse.y);
	mouse = App->render->ScreenToWorld(mouse.x, mouse.y);
	// TODO 0 ---------------------- Nothing to do here, just getting you in context
	// Every time we press leftclick button, we create a rect. The we check all entities with
	// selectable bool activated. If selectable entity is inside the rectangle, we turn their
	// isSelected bool to true

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->screen_click)
	{
		App->render->DrawQuad({ origin.x, origin.y, mouse.x - origin.x, mouse.y - origin.y }, 0, 200, 0, 100, false);
		App->render->DrawQuad({ origin.x, origin.y, mouse.x - origin.x, mouse.y - origin.y }, 0, 200, 0, 50);
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && App->input->screen_click)
	{
		player_selected = nullptr;
		portrait_entity = nullptr;
		App->input->GetMousePosition(origin.x, origin.y);
		origin = App->render->ScreenToWorld(origin.x, origin.y);
		bool loop = true;
		SDL_Rect rect;
		for (int i = 0; i < App->entity->player_dyn_ent.size() && loop; ++i)
		{
			it = App->entity->player_dyn_ent[i];
			it->isSelected = false;
			rect = it->GetAnimation()->GetCurrentSize();
			rect.x = it->position.x;
			rect.y = it->position.y;
			rect.w /= 3;
			rect.h /= 3;
		//	App->render->DrawQuad(rect, 255, 0, 0, 100);
			if (origin.x > (it->position.x - rect.w) && origin.x < (it->position.x + rect.w) && origin.y >(it->position.y - rect.h) && origin.y < (it->position.y + rect.h))
			{
				it->isSelected = true;
				player_selected = it;
				portrait_entity = it;
				loop = false;
			}
		}
		//LOG("buildings %d", App->entity->player_stat_ent.size());
		for (int i = 0; i < App->entity->player_stat_ent.size(); ++i)
		{
			it = App->entity->player_stat_ent[i];

			it->isSelected = false;
			rect = it->GetAnimation()->GetCurrentSize();
			rect.x = it->position.x;
			rect.y = it->position.y;
			rect.w /= 3;
			rect.h /= 3;
			if (origin.x > (it->position.x - rect.w) && origin.x < (it->position.x + rect.w) && origin.y >(it->position.y - rect.h) && origin.y < (it->position.y + rect.h))
			{
				it->isSelected = true;
				player_selected = it;
				portrait_entity = it;
			}
		}
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && App->input->screen_click)
	{
		App->input->GetMousePosition(origin.x, origin.y);
		origin = App->render->ScreenToWorld(origin.x, origin.y);
		bool loop = true;
		SDL_Rect rect;
		for (int i = 0; i < App->entity->ai_dyn_ent.size() && loop; ++i)
		{
			it = App->entity->ai_dyn_ent[i];
			rect = it->GetAnimation()->GetCurrentSize();
			rect.x = it->position.x;
			rect.y = it->position.y;
			rect.w /= 3;
			rect.h /= 3;
			//App->render->DrawQuad(rect, 255, 0, 0, 100);
			if (origin.x > (it->position.x - rect.w) && origin.x < (it->position.x + rect.w) && origin.y >(it->position.y - rect.h) && origin.y < (it->position.y + rect.h))
			{
				it->isSelected = true;
				ai_selected = it;
				loop = false;
			}
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && App->input->screen_click)
	{
		uint count = 0;
		gatherer_counter = 0u;
		for (int i = 0; i < App->entity->player_dyn_ent.size() && count < 30; i++) {
			it = App->entity->player_dyn_ent[i];

			if (it != player_selected)
				it->isSelected = false;
			int x = it->position.x, y = it->position.y;
			// We store x and y positions
			// Now we check if it's inside the rect, so we can "select this entity"
			if (x > origin.x&& x < mouse.x) {
				if (y < origin.y && y > mouse.y)
				{
					it->isSelected = true;
					count += 1;
				}
				else if (y > origin.y&& y < mouse.y)
				{
					it->isSelected = true;
					count += 1;
				}
			}
			else if (x < origin.x && x > mouse.x) {
				if (y < origin.y && y > mouse.y)
				{
					it->isSelected = true;
					count += 1;
				}
				else if (y > origin.y&& y < mouse.y)
				{
					it->isSelected = true;
					count += 1;
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

			if (it->isSelected && it->name == p2SString("human_gatherer"))
			{
				gatherer_counter += 1u;
			}

			if (it->isSelected) portrait_entity = it;
		}

		uint size = selected.size();
		if (gatherer_counter < uint(((size) / 2) + 1))
		{
			for (list<j1Entity*>::iterator ite = selected.begin(); ite != selected.end() && gatherer_counter > 0u; ++ite)
			{
				it = *ite;
				if (it->name == p2SString("human_gatherer"))
				{
					it->isSelected = false;
					gatherer_counter -= 1u;
					selected.erase(ite);
				}
			}
		}
		else if (gatherer_counter > uint(size / 2))
		{
			size = size - gatherer_counter;
			for (list<j1Entity*>::iterator ite = selected.begin(); ite != selected.end() && size > 0u; ++ite)
			{
				it = *ite;
				if (it->name != p2SString("human_gatherer"))
				{
					it->isSelected = false;
					size -= 1u;
					selected.erase(ite);
				}
			}
		}

		if (selected.size() > 0)
		{
			player_selected = selected.front();
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

