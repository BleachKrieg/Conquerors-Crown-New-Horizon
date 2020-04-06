#ifndef __HUMANFOOTMAN_H__
#define __HUMANFOOTMAN_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "DynamicEnt.h"

class HumanFootman : public DynamicEnt
{
public:
	// Constructor
	HumanFootman(int posx, int posy);

	// Destructor
	~HumanFootman();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate(float dt);

	bool CleanUp();

};

#endif // !__HUMANFOOTMAN_H__
