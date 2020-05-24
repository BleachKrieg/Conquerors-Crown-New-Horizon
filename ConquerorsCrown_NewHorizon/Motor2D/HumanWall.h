#ifndef __HUMANWALL_H__
#define __HUMANWALL_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "StaticEnt.h"

enum wall_states
{
	ST_WALL_PREVIEW,
	ST_WALL_FINISHED,
	ST_WALL_DESTROYED,
	ST_WALL_AUTOMATIC
};

class Human_Wall : public StaticEnt
{
public:
	// Constructor
	Human_Wall(int posx, int posy);

	// Destructor
	~Human_Wall();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	bool CleanUp();

	void CheckWalkable(iPoint map);

	// Animation
	void checkAnimation(float dt);

public:

	// Animations
	Animation inconstruction;
	Animation finishedconst;
	SDL_Rect Construction;
	SDL_Rect Created;
	wall_states	actualState;
};
#endif // __WALL_H__
