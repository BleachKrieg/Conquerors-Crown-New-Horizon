#ifndef __TUTORIAL_H__
#define __TUTORIAL_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "StaticEnt.h"
#include "EntityRequest.h"


class j1Tutorial :public j1Module
{
public:
	// Constructor
	j1Tutorial();

	// Destructor
	~j1Tutorial();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	bool CleanUp();

};
#endif // __TUTORIAL_H__

