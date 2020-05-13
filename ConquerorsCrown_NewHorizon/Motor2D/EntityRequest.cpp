#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanBarracks.h"

#include "j1App.h"
#include<stdio.h>
#include "p2Log.h"
#include "j1Textures.h"
#include "Brofiler/Brofiler.h"
#include "EntityRequest.h"

EntityRequest::EntityRequest()
{
	name.create("entity request");
}

EntityRequest::~EntityRequest()
{}

bool EntityRequest::Awake(pugi::xml_node& config)
{

	return true;
}

bool EntityRequest::Start()
{


	return true;
}

bool EntityRequest::CleanUp()
{

	return true;
}

bool EntityRequest::Update(float dt)
{
	for (int i = 0; i < Queue.size(); i++)
	{
		if (Queue[i]->timer.ReadSec() >= Queue[i]->time)
		{
			if (Queue[i]->type == Petition::SPAWN)
			{
				switch (Queue[i]->spawn)
				{
				case SpawnTypes::SWORDMAN:
					App->entity->CreateEntity(DynamicEnt::DynamicEntityType::HUMAN_FOOTMAN, Queue[i]->pos.x, Queue[i]->pos.y);
					break;
				case SpawnTypes::ARCHER:
					App->entity->CreateEntity(DynamicEnt::DynamicEntityType::HUMAN_ARCHER, Queue[i]->pos.x, Queue[i]->pos.y);
					break;
				case SpawnTypes::GATHERER:
					App->entity->CreateEntity(DynamicEnt::DynamicEntityType::HUMAN_GATHERER, Queue[i]->pos.x, Queue[i]->pos.y);
					break;
				case SpawnTypes::TROLL:
					App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_TROLL, Queue[i]->pos.x, Queue[i]->pos.y);
					break;
				case SpawnTypes::OGRE:
					App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_OGRE, Queue[i]->pos.x, Queue[i]->pos.y);
					break;
				case SpawnTypes::GRUNT:
					App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_GRUNT, Queue[i]->pos.x, Queue[i]->pos.y);
					break;
				}
			}
			Queue.erase(Queue.begin() + i);
			i--;
		}
	}
	return true;
}

bool EntityRequest::PostUpdate(float dt)
{

	return true;
}

void EntityRequest::AddRequest(Petition type, float time, SpawnTypes spawn, iPoint pos)
{
	QueueItem* item = new QueueItem();

	switch (type)
	{
	case Petition::NONE: 
		break;
	case Petition::SPAWN: 
		item->time = time;
		item->timer.Start();
		item->type = type;
		item->spawn = spawn;
		item->pos = pos;
		break;
	case Petition::UPGRADE: 
		break;
	}

	Queue.push_back(item);
}

