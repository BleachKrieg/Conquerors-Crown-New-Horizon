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
#include "j1Pathfinding.h"
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

	next_wave = 90;
	spawn_counter = 0;
	troll_counter = 0;
	grunt_counter = 0;
	ogre_counter = 0;
	wave_ongoing = false;
	max_waves = 7;

	troll_value = 15;
	grunt_value = 10;
	ogre_value = 30;
	spawn_buildings = 0;

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
	//wave_ongoing = true;
	if(App->scene->current_scene == scenes::ingame)
	{ 
		if (wave_ended.ReadSec() > next_wave && wave_ongoing == false && current_wave < max_waves) 
		{
			if (!spawn1->path.empty() && !spawn1->path.empty() && !spawn1->path.empty())
			{
				StartWave(current_wave);
			}
		}
		else if (spawn_cooldown.ReadSec() > 0.75 && wave_ongoing == true)
		{
			if (!spawn1->path.empty() && !spawn1->path.empty() && !spawn1->path.empty())
			{
				StartWave(current_wave);
			}
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
				
				if (dis_to_1 > 0)
				{
					iPoint origin = App->map->WorldToMap(spawn1->position.x, spawn1->position.y);
					iPoint destination = App->map->WorldToMap(spawn1->targetpos.x, spawn1->targetpos.y);
					App->pathfinding->RequestPath(origin, destination, nullptr, spawn1);;
				}
				if (dis_to_2 > 0)
				{
					iPoint origin = App->map->WorldToMap(spawn2->position.x, spawn2->position.y);
					iPoint destination = App->map->WorldToMap(spawn2->targetpos.x, spawn2->targetpos.y);
					App->pathfinding->RequestPath(origin, destination, nullptr, spawn2);;
				}
				if (dis_to_3 > 0)
				{
					iPoint origin = App->map->WorldToMap(spawn3->position.x, spawn3->position.y);
					iPoint destination = App->map->WorldToMap(spawn3->targetpos.x, spawn3->targetpos.y);
					App->pathfinding->RequestPath(origin, destination, nullptr, spawn3);;
				}
			}
		}
		else 
		{
			if (spawn1->target->life_points <= 0)spawn1->target = nullptr;
			if (spawn2->target->life_points <= 0)spawn2->target = nullptr;
			if (spawn3->target->life_points <= 0)spawn3->target = nullptr;

		}
		
		if (App->scene->debug == true)
		{
			App->render->DrawQuad({ spawn1->position.x, spawn1->position.y, 30, 30 }, 255, 0, 0);
			App->render->DrawQuad({ spawn2->position.x, spawn2->position.y, 30, 30 }, 255, 0, 0);
			App->render->DrawQuad({ spawn3->position.x, spawn3->position.y, 30, 30 }, 255, 0, 0);
		}


		for (uint i = 0; i < spawn1->path.size(); ++i)
		{
			iPoint nextPoint = App->map->MapToWorld(spawn1->path.at(i).x, spawn1->path.at(i).y);
			if (App->scene->debug)
			{
				
				App->render->DrawQuad({ nextPoint.x + 14, nextPoint.y + 14, 6, 6 }, 200, 0, 0, 100);
			}
		}

		for (uint i = 0; i < spawn2->path.size(); ++i)
		{
			iPoint nextPoint = App->map->MapToWorld(spawn2->path.at(i).x, spawn2->path.at(i).y);
			if (App->scene->debug)
			{

				App->render->DrawQuad({ nextPoint.x + 14, nextPoint.y + 14, 6, 6 }, 200, 0, 0, 100);
			}
		}

		for (uint i = 0; i < spawn3->path.size(); ++i)
		{
			iPoint nextPoint = App->map->MapToWorld(spawn3->path.at(i).x, spawn3->path.at(i).y);
			if (App->scene->debug)
			{

				App->render->DrawQuad({ nextPoint.x + 14, nextPoint.y + 14, 6, 6 }, 200, 0, 0, 100);
			}
		}

		if (spawn1->bulding->life_points <= 0 && spawn2->bulding->life_points <= 0 && spawn3->bulding->life_points <= 0)
		{
			spawn_buildings = 0;
		}
		else if ((spawn1->bulding->life_points <= 0 && spawn2->bulding->life_points <= 0) || (spawn1->bulding->life_points <= 0 && spawn3->bulding->life_points <= 0) || (spawn2->bulding->life_points <= 0 && spawn3->bulding->life_points <= 0))
		{
			spawn_buildings = 1;
		}
		else if (spawn1->bulding->life_points || spawn2->bulding->life_points <= 0 || spawn3->bulding->life_points <= 0)
		{
			spawn_buildings = 2;
		}
		else { spawn_buildings = 3; }
	}
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
	
	//int total_spawns = 2 + 9 * wave;

	int wave_value = 30 + 80 * wave;
	if (spawn_buildings == 2) { wave_value = wave_value - 20 * wave; }
	else if (spawn_buildings == 1) { wave_value = wave_value - 60 * wave; }
	else if (spawn_buildings == 0) { wave_value = 0; }

	if (wave_ongoing == false) {
		if (wave == 1) {
			trolls = 12; ogres = 0; grunts = 0;
			if (spawn_buildings == 3) { trolls = 12; ogres = 0; grunts = 0; }
			else if (spawn_buildings == 2) { trolls = 10; ogres = 0; grunts = 0; }
			else if (spawn_buildings == 1) { trolls = 8; ogres = 0; grunts = 0; }
			else { trolls = 0; ogres = 0; grunts = 0; }
		}
		else if (wave == 5) { 
			if (spawn_buildings == 3) { trolls = 0; ogres = 15; grunts = 0; }
			else if (spawn_buildings == 2) { trolls = 0; ogres = 12; grunts = 0; }
			else if (spawn_buildings == 1) { trolls = 0; ogres = 9; grunts = 0; }
			else { trolls = 0; ogres = 0; grunts = 0; }
		}
		else if (wave == 2) {
			int max_trolls, max_grunts;
			trolls = grunts = ogres = 0;
			do {
				max_trolls = wave_value / troll_value;
				if (max_trolls > 0) { trolls = trolls + rand() % max_trolls + 1; }
				wave_value -= trolls * troll_value;
				max_grunts = wave_value / grunt_value;
				if (max_grunts > 0) { grunts = grunts + rand() % max_grunts + 1; }
				wave_value -= grunts * grunt_value;
			} while (wave_value >= grunt_value);
		}
		else {
			int max_trolls, max_grunts, max_ogres;
			trolls = grunts = ogres = 0;
			do {
				max_trolls = wave_value / troll_value;
				if (max_trolls > 0) { trolls = trolls + rand() % max_trolls + 1; }
				wave_value -= trolls * troll_value;
				max_grunts = wave_value / grunt_value;
				if (max_grunts > 0) { grunts = grunts + rand() % max_grunts + 1; }
				wave_value -= grunts * grunt_value;
				max_ogres = wave_value / ogre_value;
				if (max_ogres > 0) { ogres = ogres + rand() % max_ogres + 1; }
				wave_value -= ogres * ogre_value;
				LOG("Value: %i, Trolls: %i, Grunts: %i, Ogres %i", wave_value, trolls, grunts, ogres);
			} while (wave_value >= grunt_value);
		}
	}
	int total_spawns = trolls + ogres + grunts;

	wave_ongoing = true;
	spawn_cooldown.Start();

	int spawns;
	if ((total_spawns - spawn_counter) >= 3) { spawns = 3; }
	else { spawns = (total_spawns - spawn_counter); }
	if (spawns > spawn_buildings) { spawns = spawn_buildings; }
	spawn_counter += spawns;

	for (int i = 1; i <= spawns; i++) {
		if (i % 3 == 1 && spawn1->bulding->life_points > 0)
		{
			bool enemy_spawned = false;
			do {
				int r = rand() % 3 + 1;
				switch (r) {
				case 1:
					if (troll_counter < trolls) { SpawnTroll(spawn1); troll_counter++; enemy_spawned = true; }
					break;
				case 2:
					if (grunt_counter < grunts) { SpawnGrunt(spawn1); grunt_counter++; enemy_spawned = true; }
					break;
				case 3:
					if (ogre_counter < ogres) { SpawnOgre(spawn1); ogre_counter++; enemy_spawned = true; }
					break;
				}
			} while (enemy_spawned == false);
		}
		else if (i % 3 == 2 && spawn2->bulding->life_points > 0) {
			bool enemy_spawned = false;
			do {
				int r = rand() % 3 + 1;
				switch (r) {
				case 1:
					if (troll_counter < trolls) { SpawnTroll(spawn2); troll_counter++; enemy_spawned = true; }
					break;
				case 2:
					if (grunt_counter < grunts) { SpawnGrunt(spawn2); grunt_counter++; enemy_spawned = true; }
					break;
				case 3:
					if (ogre_counter < ogres) { SpawnOgre(spawn2); ogre_counter++; enemy_spawned = true; }
					break;
				}
			} while (enemy_spawned == false);
		}																													
		else if (i % 3 == 0 && spawn3->bulding->life_points > 0) {
			bool enemy_spawned = false;
			do {
				int r = rand() % 3 + 1;
				switch (r) {
				case 1:
					if (troll_counter < trolls) { SpawnTroll(spawn3); troll_counter++; enemy_spawned = true; }
					break;
				case 2:
					if (grunt_counter < grunts) { SpawnGrunt(spawn3); grunt_counter++; enemy_spawned = true; }
					break;
				case 3:
					if (ogre_counter < ogres) { SpawnOgre(spawn3); ogre_counter++; enemy_spawned = true; }
					break;
				}
			} while (enemy_spawned == false);
		}
	}

	if (spawn_counter >= total_spawns) { FinishWave(); }
}

void j1WaveSystem::FinishWave()
{
	current_wave++;
	wave_ongoing = false;
	wave_ended.Start();
	//next_wave = 90;
	spawn_counter = 0;
	troll_counter = 0;
	grunt_counter = 0;
	ogre_counter = 0;
	if (App->scene->ingameTextWave != nullptr)
		App->scene->ingameTextWave->SetText(to_string(current_wave - 1).c_str());
}

bool j1WaveSystem::SpawnTroll(SpawnPoint* spawn)
{
	TrollEnemy*	temp = (TrollEnemy*)App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_TROLL, spawn->position.x, spawn->position.y);
	temp->spawn = spawn;

	for (uint i = 0; i < spawn->path.size(); ++i)
	{
		iPoint point(spawn->path.at(i).x, spawn->path.at(i).y);
		temp->path.push_back(point);
	}
	return true;
}

bool j1WaveSystem::SpawnOgre(SpawnPoint* spawn)
{
	OgreEnemy*	temp = (OgreEnemy*)App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_OGRE, spawn->position.x, spawn->position.y);
	temp->spawn = spawn;

	for (uint i = 0; i < spawn->path.size(); ++i)
	{
		iPoint point(spawn->path.at(i).x, spawn->path.at(i).y);
		temp->path.push_back(point);
	}
	return true;
}

bool j1WaveSystem::SpawnGrunt(SpawnPoint* spawn)
{
	GruntEnemy*	temp = (GruntEnemy*)App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_GRUNT, spawn->position.x, spawn->position.y);
	temp->spawn = spawn;

	for (uint i = 0; i < spawn->path.size(); ++i)
	{
		iPoint point(spawn->path.at(i).x, spawn->path.at(i).y);
		temp->path.push_back(point);
	}
	return true;
}

bool j1WaveSystem::CreateSpawnBuildings()
{
	spawn1->bulding = App->entity->CreateStaticEntity(StaticEnt::StaticEntType::enemy_barrack, spawn1->position.x, spawn1->position.y);
	spawn2->bulding = App->entity->CreateStaticEntity(StaticEnt::StaticEntType::enemy_barrack, spawn2->position.x, spawn2->position.y);
	spawn3->bulding = App->entity->CreateStaticEntity(StaticEnt::StaticEntType::enemy_barrack, spawn3->position.x, spawn3->position.y);

	spawn_buildings = 3;

	return true;
}