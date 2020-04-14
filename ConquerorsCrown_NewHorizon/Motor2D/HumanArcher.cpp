#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanArcher.h"
#include "DynamicEnt.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Input.h"
#include "J1GroupMov.h"
#include <math.h>

HumanArcher::HumanArcher(int posx, int posy) : DynamicEnt(DynamicEntityType::HUMAN_ARCHER)
{
	name.create("human_archer");

	// TODO: Should get all the DATA from a xml file
	speed = { NULL, NULL };
	life_points = 100;
	attack_vision = 200;
	attack_range = 140;
	time_attack = 1000;
	attack_damage = 16;
	vision = 26;
	body = 13;
	position.x = posx;
	position.y = posy;
	orientation = SDL_FLIP_NONE;
	to_delete = false;
	can_attack = true;
	isSelected = false;
	selectable = true;
	following_target = false;
	player_order = false;
	team = TeamType::PLAYER;
	target_entity = NULL;
	state = DynamicState::IDLE;

	// TODO ------------------------------------------
}

HumanArcher::~HumanArcher() {}

bool HumanArcher::Start()
{

	list<Animation*>::iterator animations_list;
	animations_list = App->entity->archer_animations.begin();
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

bool HumanArcher::Update(float dt)
{
	BROFILER_CATEGORY("ArcherUpdate", Profiler::Color::BlanchedAlmond);

	// Speed resetted to 0 each iteration
	speed = { NULL, NULL };
	origin = App->map->WorldToMap(position.x, position.y);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected)
		life_points = 0;

	Movement();

	if (life_points <= 0)
		state = DynamicState::DYING;

	switch (state)
	{
	case DynamicState::IDLE:
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
		break;
	case DynamicState::DYING:
		to_delete = true;
		break;
	}
	
	//App->render->DrawQuad({ (int)position.x, (int)position.y, 10, 10 }, 200, 200, 0);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->arch_man_tex, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	return true;
}

bool HumanArcher::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Archer_PostUpdate", Profiler::Color::BurlyWood)

	return true;
}

bool HumanArcher::CleanUp()
{
	close_entity_list.clear();
	colliding_entity_list.clear();
	path.Clear();
	name.Clear();
	return true;
}