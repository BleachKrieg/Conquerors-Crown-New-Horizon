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

	spawn_point1.x = config.child("spawnpoints").child("spawnpoint1").attribute("x").as_int();
	spawn_point1.y = config.child("spawnpoints").child("spawnpoint1").attribute("y").as_int();
	spawn_point2.x = config.child("spawnpoints").child("spawnpoint2").attribute("x").as_int();
	spawn_point2.y = config.child("spawnpoints").child("spawnpoint2").attribute("y").as_int();
	spawn_point3.x = config.child("spawnpoints").child("spawnpoint3").attribute("x").as_int();
	spawn_point3.y = config.child("spawnpoints").child("spawnpoint3").attribute("y").as_int();

	return ret;
}

// Called before the first frame
bool j1WaveSystem::Start()
{
	bool ret = true;

	LOG("Start Wave System");

	current_wave = 1;
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

	if (wave_ended.ReadSec() > 10 && wave_ongoing == false) { 
		StartWave(current_wave);
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
	int spawns = 9 + 6 * wave;
	wave_ongoing = true;
	for (int i = 1; i <= spawns; i++) {
		if (i % 3 == 1) { App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::TROLL, { spawn_point1.x, spawn_point1.y }); }
		else if (i % 3 == 2) { App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::TROLL, { spawn_point2.x, spawn_point2.y }); }
		else if (i % 3 == 0) { App->requests->AddRequest(Petition::SPAWN, 0.f, SpawnTypes::TROLL, { spawn_point3.x, spawn_point3.y }); }
	}
}
