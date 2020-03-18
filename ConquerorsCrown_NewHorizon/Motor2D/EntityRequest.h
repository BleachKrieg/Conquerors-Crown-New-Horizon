#ifndef __j1EntityRequest_H__
#define __j1EntityRequest_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"

#include <list>
#include <vector>

using namespace std;
class j1Timer;

enum class Petition
{
	NONE,
	UPGRADE,
	SPAWN
};
enum class SpawnTypes
{
	NONE,
	SWORDMAN,
	ARCHER,
	PEASANT
};

struct QueueItem
{
	int			time;
	j1Timer		timer;
	Petition	type;
	SpawnTypes	spawn;
	iPoint		pos;
};

class EntityRequest : public j1Module
{
public:
	// Constructor
	EntityRequest();

	// Destructor
	~EntityRequest();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	// Delete an entity
	void AddRequest(Petition type, float time, SpawnTypes spawn = SpawnTypes::NONE, iPoint pos = { 0, 0 });

private:
	vector<QueueItem*>	Queue;
	
};

#endif // __j1EntityManager_H__