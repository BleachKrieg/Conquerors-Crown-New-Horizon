#ifndef __RESOURCEENTITIES_H__
#define __RESOURCEENTITIES_H__

#include "j1Module.h"
#include "p2Point.h"
#include "StaticEnt.h"
#include "j1PerfTimer.h"

class ResourceEntity : public StaticEnt
{
public:

	// Constructor
	ResourceEntity(int posx, int posy, uint enter_type);

	// Destructor
	~ResourceEntity();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all updates
	bool PostUpdate(float dt);

	bool CleanUp();

	enum class RESOURCE_ENTITY_TYPE
	{
		NONE,
		TREE,
		MINE,
		QUARRY,
	};

public:
	uint& GetResourceType();

private:
	RESOURCE_ENTITY_TYPE	type;
	j1PerfTimer				timer;
	float					task_time;
};

#endif // !__RESOURCEENTITIES_H__