#include "ResourceEntities.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "StaticEnt.h"
#include "j1Map.h"
#include "Brofiler/Brofiler.h"

ResourceEntity::ResourceEntity(int posx, int posy, uint enter_type) : StaticEnt(StaticEntType::Resource)
{
	name.create("resource");
	type = (RESOURCE_ENTITY_TYPE)enter_type;
	if (type == RESOURCE_ENTITY_TYPE::MINE)
		task_time = 30000.0F;
	if (type == RESOURCE_ENTITY_TYPE::QUARRY)
		task_time = 25000.0F;
	if (type == RESOURCE_ENTITY_TYPE::TREE)
		task_time = 20000.0F;
	position.x = posx;
	position.y = posy;

	active = false;
}

ResourceEntity::~ResourceEntity() {}

bool ResourceEntity::Start()
{
	iPoint pos = App->map->WorldToMap((int)position.x, (int)position.y);
	App->pathfinding->ChangeWalkability(pos, 2);
	return true;
}

bool ResourceEntity::Update(float dt)
{
	BROFILER_CATEGORY("Update_ResourceEntity", Profiler::Color::BlanchedAlmond)

	return true;
}

bool ResourceEntity::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Postupdate_ResourceEntity", Profiler::Color::BurlyWood)

	return true;
}

bool ResourceEntity::CleanUp()
{
	iPoint pos = App->map->WorldToMap((int)position.x, (int)position.y);
	App->pathfinding->ChangeWalkability(pos, 1);
	return true;
}

uint& ResourceEntity::GetResourceType()
{
	return (uint&)type;
}