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
	attack_vision = 200;
	vision = 26;
	body = 13;
	position.x = posx;
	position.y = posy;
	orientation = SDL_FLIP_NONE;
	to_delete = false;
	isSelected = false;
	selectable = true;
	following_target = false;
	team = TeamType::PLAYER;

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

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected)
		life_points = 0;

	if (life_points <= 0u)
		to_delete = true;
	Movement();

	if (isSelected)
		App->render->DrawCircle((int)position.x + 5, (int)position.y + 5, 10, 0, 200, 0, 200);

	//App->render->DrawQuad({ (int)position.x, (int)position.y, 10, 10 }, 200, 200, 0);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->foot_man_tex, (int)(position.x - (*r).w/2), (int)(position.y - (*r).h/2), r, 1.0f, 1.0f, orientation);
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
	path.Clear();
	name.Clear();
	return true;
}