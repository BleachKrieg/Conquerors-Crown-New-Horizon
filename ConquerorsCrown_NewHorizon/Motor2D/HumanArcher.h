#ifndef __HUMANARCHER_H__
#define __HUMANARCHER_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "DynamicEnt.h"

class HumanArcher : public DynamicEnt
{
public:
	// Constructor
	HumanArcher(int posx, int posy);

	// Destructor
	~HumanArcher();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all updates
	bool PostUpdate(float dt);

	// Clean
	bool CleanUp();

//	void SaveNeighbours(list<j1Entity*>*, list<j1Entity*>*);

private:
	Animation moving_up;
	Animation moving_diagonal_up;
	Animation moving_right;
	Animation moving_diagonal_down;
	Animation moving_down;

};

#endif // !__HUMANARCHER_H__
