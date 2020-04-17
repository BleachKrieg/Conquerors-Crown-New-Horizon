#ifndef __HUMANGATHERER_H__
#define __HUMANGATHERER_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "DynamicEnt.h"

class HumanGatherer : public DynamicEnt
{
public:
	// Constructor
	HumanGatherer(int posx, int posy);

	// Destructor
	~HumanGatherer();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all the updates
	bool PostUpdate(float dt);

	bool CleanUp();

private:
	j1Entity* town_hall;
	j1Entity* work_space;

};


#endif // !__HUMANGATHERER_H__
