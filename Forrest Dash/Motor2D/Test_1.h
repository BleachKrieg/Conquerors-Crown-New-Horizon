#ifndef __TEST_1_H__
#define __TEST_1_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"

enum states_test_1
{
	ST_TEST_1_IDLE,
	ST_TEST_1_MOVE,
	ST_TEST_1_DEAD
};

class Test_1 : public j1Entity
{
public:
	// Constructor
	Test_1(int posx, int posy);

	// Destructor
	~Test_1();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Animation
	void CheckAnimation(float dt);

public:
	// Animations
	states_test_1	actualState;
	Animation* current_animation = nullptr;
	Animation idle;
	Animation right;
	Animation left;
	Animation up;
	Animation down;
	Animation dead;
};

#endif // __TEST_1_H__
