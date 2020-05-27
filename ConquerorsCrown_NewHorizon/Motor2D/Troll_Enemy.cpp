#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "Troll_Enemy.h"
#include "DynamicEnt.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "J1GroupMov.h"
#include "Emiter.h"
#include "ParticleSystem.h"
#include "FoWManager.h"


#include <math.h>

TrollEnemy::TrollEnemy(int posx, int posy) : DynamicEnt(DynamicEntityType::ENEMY_TROLL)
{
	name.create("enemy_troll");

	
	// TODO: Should get all the DATA from a xml file
	speed = { NULL, NULL };
	life_points = 80;
	attack_vision = 200;
	attack_range = 100;
	time_attack = 1400;
	attack_damage = 12;
	vision = 20;
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
	entity_type = DynamicEntityType::ENEMY_TROLL;
	speed_modifier = 0.7;


	// TODO ------------------------------------------
}

TrollEnemy::~TrollEnemy() {}

bool TrollEnemy::Start()
{
	list<Animation*>::iterator animations_list;
	animations_list = App->entity->troll_animations.begin();
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

	particleSystem = App->entity->CreateParticleSys(position.x, position.y);
	Animation anim;
	anim.PushBack(SDL_Rect{ 0, 96, 32, 32 }, 1, 0, 0, 0, 0);

	anim.Reset();
	Emiter emiter(position.x, position.y, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 2, 2, nullptr, App->entity->arrow, anim, false);
	particleSystem->PushEmiter(emiter );
	particleSystem->Desactivate();

	spawn = nullptr;
	time = 10;
	followpath = 0;
	change_direction = true;

	return true;
}

bool TrollEnemy::Update(float dt)
{
	BROFILER_CATEGORY("Update_TrollEnemy", Profiler::Color::BlanchedAlmond);

	speed = { 0, 0 };
	origin = App->map->WorldToMap(position.x, position.y);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;
	

	AttackTarget(entity_type);

	Movement(dt);

	if (particleSystem != nullptr)
		particleSystem->Move(position.x, position.y);

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
		else
		{
			spawn = App->wave->spawn1;
			int x = App->wave->spawn1->position.x;
			int y = App->wave->spawn1->position.y;
			int distance = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));
			x = App->wave->spawn2->position.x;
			y = App->wave->spawn2->position.y;
			int distance2 = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));
			if (distance > distance2)
			{
				distance = distance2;
				spawn = App->wave->spawn2;
			}
			x = App->wave->spawn3->position.x;
			y = App->wave->spawn3->position.y;
			 distance2 = sqrt(pow((position.x - x), 2) + pow((position.y - y), 2));
			if (distance > distance2)
			{
				distance = distance2;
				spawn = App->wave->spawn3;
			}
					

				
		}
	
	}
	time = idletime.ReadSec();

	if (state != DynamicState::INTERACTING)
		if (particleSystem->IsActive())
			particleSystem->Desactivate();
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
		if (particleSystem != nullptr)
		{
			if (!particleSystem->IsActive())
			{
				particleSystem->Activate();
			}
				float xvec, yvec, ProjTime = 1;
				iPoint destiny = App->map->WorldToMap(target_entity->position.x, target_entity->position.y);
				iPoint start = App->map->WorldToMap(position.x + 15, position.y + 15);


				fPoint vec(destiny.x - start.x, destiny.y - start.y);

				float norm = sqrt(pow(vec.x, 2) + pow(vec.y, 2));
				yvec = (vec.y / norm);
				xvec = (vec.x / norm);


				xvec *= 3;
				yvec *= 3;
				if (norm > 0)
				{
					if (abs(xvec) > abs(yvec))
						ProjTime = norm / abs(xvec);
					else
						ProjTime = norm / abs(yvec);
					ProjTime *= 0.4;
				}
				else {
					ProjTime = 1;
				}

				if (particleSystem->emiterVector.size() > 0)
				{
					particleSystem->emiterVector[0].SetSpeed(xvec, yvec);
					particleSystem->emiterVector[0].SetMaxTime(ProjTime);
				}

		}
		break;
	case DynamicState::DYING:
		if (particleSystem->IsActive())
		{
			particleSystem->Desactivate();
		}
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
	iPoint mapPos = App->map->WorldToMap((int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2));
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
	if (fogId == -1 && shroudId == -1 || App->scene->debug)
	App->render->Blit(App->entity->troll_tex, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	return true;
}

bool TrollEnemy::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostUpdate_TrollEnemy", Profiler::Color::BurlyWood)

		return true;
}

bool TrollEnemy::CleanUp()
{
	close_entity_list.clear();
	colliding_entity_list.clear();
	particleSystem->to_delete = true;

	path.clear();
	name.Clear();
	return true;
}