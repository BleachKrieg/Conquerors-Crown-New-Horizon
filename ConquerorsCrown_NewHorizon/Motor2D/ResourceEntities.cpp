#include "ResourceEntities.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "StaticEnt.h"
#include "j1Map.h"
#include "Brofiler/Brofiler.h"

ResourceEntity::ResourceEntity(int posx, int posy, uint enter_type) : StaticEnt(StaticEntType::Resource)
{
	type = (RESOURCE_ENTITY_TYPE)enter_type;
	if (type == RESOURCE_ENTITY_TYPE::MINE)
		name.create("mine");
	else if (type == RESOURCE_ENTITY_TYPE::QUARRY)
		name.create("quarry");
	else if (type == RESOURCE_ENTITY_TYPE::TREE)
		name.create("tree");
	else
		name.create("resource");

	position.x = posx;
	position.y = posy;

}

ResourceEntity::~ResourceEntity() {}

bool ResourceEntity::Start()
{
	iPoint pos = App->map->WorldToMap((int)position.x, (int)position.y);
	App->pathfinding->ChangeWalkability(pos, 2);
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
