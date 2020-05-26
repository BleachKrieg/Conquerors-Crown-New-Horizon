#ifndef __HUMANKNIGHT_H__
#define __HUMANKNIGHT_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "DynamicEnt.h"

class HumanKnight : public DynamicEnt
{
public:
	// Constructor
	HumanKnight(int posx, int posy);

	// Destructor
	~HumanKnight();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate(float dt);

	bool CleanUp();

};

#endif // !__HUMANFOOTMAN_H__
