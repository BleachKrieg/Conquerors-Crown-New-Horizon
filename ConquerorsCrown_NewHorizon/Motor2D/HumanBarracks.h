#ifndef __HUMANBARRACKS_H__
#define __HUMANBARRACKS_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "StaticEnt.h"
#include "EntityRequest.h"

enum Barrack_states
{
	ST_BARRACK_PREVIEW,
	ST_BARRANCK_IN_CONSTRUCTION,
	ST_BARRACK_FINISHED,
	ST_BARRACK_DESTROYED
};

class HumanBarracks : public StaticEnt
{
public:
	// Constructor
	HumanBarracks(int posx, int posy);

	// Destructor
	~HumanBarracks();

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

	// UI
	void CreateBarrackUI();
	void DeleteBarracksUI();

public:
	// Animations
	Animation* current_animation = nullptr;
	Animation inconstruction;
	Animation finishedconst;
	SDL_Rect Construction;
	SDL_Rect Created;
    Barrack_states	actualState;
	
	// Stats
	int attackrange;
	int collrange;

	// Queue
	float timer_queue;
	j1Timer start_queue;

	struct QueueTroop
	{
		int			time;
		j1Timer		timer;
	};

	vector<QueueTroop*>	Troop;

	void CheckQueue();
	
	// UI
	GuiItem* Button_Create_Footman;
	GuiItem* Icons;
};
#endif // __TEST_1_H__
