#ifndef __GRUNT_ENEMY_H__
#define __GRUNT_ENEMY_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "DynamicEnt.h"
#include "j1WaveSystem.h"

struct SpawnPoint;

class GruntEnemy : public DynamicEnt
{
public:

	// Constructor
	GruntEnemy(int posx, int posy);

	// Destructor
	~GruntEnemy();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate(float dt);

	bool CleanUp();
public:
	SpawnPoint* spawn;
	j1Timer idletime;
	int time;
};

#endif // !__GRUNT_ENEMY_H__