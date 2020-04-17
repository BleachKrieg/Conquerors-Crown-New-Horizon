#include "j1WaveSystem.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Fonts.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "EntityRequest.h"
#include "j1FadeToBlack.h"
#include <math.h>

j1WaveSystem::j1WaveSystem() : j1Module()
{
	name.create("wave");

}

// Destructor
j1WaveSystem::~j1WaveSystem()
{}

// Called before render is available
bool j1WaveSystem::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Loading Wave System");
	pugi::xml_node wave;
	spawn1 = new SpawnPoint();
	spawn2 = new SpawnPoint();

	spawn3 = new SpawnPoint();

	spawn1->position.x = config.child("spawnpoints").child("spawnpoint1").attribute("x").as_int();
	spawn1->position.y = config.child("spawnpoints").child("spawnpoint1").attribute("y").as_int();
	spawn2->position.x = config.child("spawnpoints").child("spawnpoint2").attribute("x").as_int();
	spawn2->position.y = config.child("spawnpoints").child("spawnpoint2").attribute("y").as_int();
	spawn3->position.x = config.child("spawnpoints").child("spawnpoint3").attribute("x").as_int();
	spawn3->position.y = config.child("spawnpoints").child("spawnpoint3").attribute("y").as_int();

	return ret;
}

// Called before the first frame
bool j1WaveSystem::Start()
{
	bool ret = true;

	LOG("Start Wave System");

	current_wave = 1;

	wave_ongoing = true;
	spawn1->target = nullptr;
	spawn2->target = nullptr;
	spawn3->target = nullptr;

	next_wave = 10;
	wave_ongoing = false;

	return ret;
}

// Called each loop iteration
bool j1WaveSystem::PreUpdate(float dt)
{
	return true;
}

// Called each loop iteration
bool j1WaveSystem::Update(float dt)
{
	bool ret = true;

	if (wave_ended.ReadSec() > 5 && wave_ongoing == false) { 
		StartWave(current_wave);
		wave_ended.Start();
	}


	if ((spawn1->target == nullptr || spawn2->target == nullptr || spawn3->target == nullptr))
	{
		if(!App->entity->player_stat_ent.empty())
		{
			spawn1->target = nullptr;
			spawn2->target = nullptr;
			spawn3->target = nullptr;

			int dis_to_1, dis_to_2, dis_to_3;
			dis_to_1 = dis_to_2 = dis_to_3 = 0;

			for (int i = 0; i < App->entity->player_stat_ent.size(); i++) {
				if (App->entity->player_stat_ent[i]->deployed == false)
					continue;
				fPoint vec1, vec2, vec3;
				vec1 = { App->entity->player_stat_ent[i]->position.x - spawn1->position.x,  App->entity->player_stat_ent[i]->position.y - spawn1->position.y };
				vec2 = { App->entity->player_stat_ent[i]->position.x - spawn2->position.x,  App->entity->player_stat_ent[i]->position.y - spawn2->position.y };
				vec3 = { App->entity->player_stat_ent[i]->position.x - spawn3->position.x,  App->entity->player_stat_ent[i]->position.y - spawn3->position.y };
				int norm1, norm2, norm3;
				norm1 = sqrt(pow((vec1.x), 2) + pow((vec1.y), 2));
				norm2 = sqrt(pow((vec2.x), 2) + pow((vec2.y), 2));
				norm3 = sqrt(pow((vec3.x), 2) + pow((vec3.y), 2));

				if (dis_to_1 == 0 || dis_to_1 > norm1)
				{
					dis_to_1 = norm1;
					spawn1->target = App->entity->player_stat_ent[i];
					spawn1->targetpos = spawn1->target->position;

				}
				if (dis_to_2 == 0 || dis_to_2 > norm2)
				{
					dis_to_2 = norm2;
					spawn2->target = App->entity->player_stat_ent[i];
					spawn2->targetpos = spawn2->target->position;
				}
				if (dis_to_3 == 0 || dis_to_3 > norm3)
				{
					dis_to_3 = norm3;
					spawn3->target = App->entity->player_stat_ent[i];
					spawn3->targetpos = spawn3->target->position;

				}

			}
		}
		
	}
	else 
	{
		if (spawn1->target->life_points <= 0)spawn1->target = nullptr;
		if (spawn2->target->life_points <= 0)spawn2->target = nullptr;
		if (spawn3->target->life_points <= 0)spawn3->target = nullptr;

	}
	
	App->render->DrawQuad({ spawn1->position.x, spawn1->position.y, 30, 30 }, 255, 0, 0);
	App->render->DrawQuad({ spawn2->position.x, spawn2->position.y, 30, 30 }, 255, 0, 0);
	App->render->DrawQuad({ spawn3->position.x, spawn3->position.y, 30, 30 }, 255, 0, 0);


	return ret;
}

// Called each loop iteration
bool j1WaveSystem::PostUpdate(float dt)
{
	return true;
}

// Called before quitting
bool j1WaveSystem::CleanUp()
{
	return true;
}

bool j1WaveSystem::Load(pugi::xml_node& data)
{
	LOG("Loading Wave state");

	return true;
}

// Save Game State
bool j1WaveSystem::Save(pugi::xml_node& data) const
{
	LOG("Saving Wave state");
	
	return true;
}

void j1WaveSystem::StartWave(int wave)
{
	
	TrollEnemy* temp;
	int spawns = 9 + 6 * wave;

	wave_ongoing = false;
	spawns = 3;
	for (int i = 1; i <= spawns; i++) {
		if (i % 3 == 1)
		{
			temp = (TrollEnemy*)App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_TROLL, spawn1->position.x, spawn1->position.y);
			temp->spawn = spawn1;
		}
		else if (i % 3 == 2) {
			temp = (TrollEnemy*)App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_TROLL, spawn2->position.x, spawn2->position.y); 
			temp->spawn = spawn2;																							
		}																													
		else if (i % 3 == 0) {																								
			temp = (TrollEnemy*)App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_TROLL, spawn2->position.x, spawn2->position.y);
			temp->spawn = spawn3;
		}
	}
}


void j1WaveSystem::FinishWave()
{
	current_wave++;
	wave_ongoing = false;
	wave_ended.Start();
	next_wave = 20;
}
