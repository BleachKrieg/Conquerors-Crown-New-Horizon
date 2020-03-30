#ifndef __TROLL_ENEMY_H__
#define __TROLL_ENEMY_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "DynamicEnt.h"

class TrollEnemy : public DynamicEnt
{
public:
	// Constructor
	TrollEnemy(int posx, int posy);

	// Destrutor
	~TrollEnemy();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate(float dt);

	bool CleanUp();

};

#endif // !__TROLL_ENEMY_H__