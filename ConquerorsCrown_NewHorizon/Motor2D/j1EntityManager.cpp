#include "j1EntityManager.h"
#include "j1Entity.h"
#include "Test_1.h"
#include "Test_3.h"
#include "HumanFootman.h"
#include "HumanArcher.h"

#include "j1App.h"
#include<stdio.h>
#include "p2Log.h"
#include "j1Textures.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager()
{
	name.create("entity_manager");
}

j1EntityManager::~j1EntityManager()
{}

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading EntityManager");

	

	return true;
}

bool j1EntityManager::Start()
{

	foot_man_tex = App->tex->Load("textures/units/Human Sprites/human_footman.png");

	return true;
}

bool j1EntityManager::CleanUp()
{
	
	list<j1Entity*>::iterator entities_list;
	j1Entity* it;

	for (entities_list = entities.begin(); entities_list != entities.end(); ++entities_list) {
		it = *entities_list;
		it->CleanUp();
		RELEASE(it);
	}

	entities.clear();

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("UpdateEntity", Profiler::Color::Bisque);

	list<j1Entity*>::iterator entities_list;
	j1Entity* it;

	for (entities_list = entities.begin(); entities_list != entities.end(); ++entities_list) {
		it = *entities_list;
		it->Update(dt);
	}

	

	return true;
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateEntity", Profiler::Color::Azure)

		list<j1Entity*>::iterator entities_list;
	j1Entity* it;

	for (entities_list = entities.begin(); entities_list != entities.end(); ++entities_list) {
		it = *entities_list;
		if (it->to_delete == true)
		{
			DeleteEntity(entities_list, it);
		}
		else
		{
			it->PostUpdate();
		}
	}

		return true;
}

j1Entity* j1EntityManager::CreateEntity(DynamicEnt::DynamicEntityType type, int posx, int posy)
{
	j1Entity* ret = nullptr;

	switch (type)
	{
	case DynamicEnt::DynamicEntityType::TEST_1: ret = new Test_1(posx, posy); break;
	case DynamicEnt::DynamicEntityType::HUMAN_FOOTMAN: ret = new HumanFootman(posx, posy); break;
	case DynamicEnt::DynamicEntityType::HUMAN_ARCHER: ret = new HumanArcher(posx, posy); break;
	}

	if (ret != nullptr)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	return ret;
}

j1Entity* j1EntityManager::CreateStaticEntity(StaticEnt::StaticEntType type, int posx, int posy)
{
	j1Entity* ret = nullptr;

	switch (type)
	{
	case StaticEnt::StaticEntType::TEST_3: ret = new Test_3(posx, posy); break;

	}

	if (ret != nullptr)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	return ret;
}

bool j1EntityManager::DeleteEntity(list<j1Entity*>::iterator entity_iterator, j1Entity* entity)
{
	entity->CleanUp();
	entities.erase(entity_iterator);

	return true;
}