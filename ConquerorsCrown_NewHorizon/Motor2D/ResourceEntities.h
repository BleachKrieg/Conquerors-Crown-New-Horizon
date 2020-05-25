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

	bool CleanUp();

	enum class RESOURCE_ENTITY_TYPE
	{
		NONE,
		TREE,
		QUARRY,
	};

private:
	RESOURCE_ENTITY_TYPE	type;
};

#endif // !__RESOURCEENTITIES_H__