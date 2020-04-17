#ifndef __HUMANTOWNHALL_H__
#define __HUMANTOWNHALL_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "StaticEnt.h"
#include "EntityRequest.h"

enum TownHall_states
{
	ST_TOWNHALL_PREVIEW,
	ST_TOWNHALL_IN_CONSTRUCTION,
	ST_TOWNHALL_FINISHED,
	ST_TOWNHALL_AUTOMATIC,
	ST_TOWNHALL_DESTROYED
};

class HumanTownHall : public StaticEnt
{
public:
	// Constructor
	HumanTownHall(int posx, int posy);

	// Destructor
	~HumanTownHall();

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
	void CreateTownHallUI();
	void DeleteTownHallUI();
	void GuiInput(GuiItem*);
	
	//Queue
	void CheckQueue();

public:
	// Animations
	Animation inconstruction;
	Animation finishedconst2;
	SDL_Rect Construction;
	SDL_Rect Created;
	TownHall_states	actualState;

	// Stats
	int attackrange;
	int collrange;

	// Queue
	float timer_queue;
	j1Timer start_queue;

	// Queue_v2
	struct QueueTroop
	{
		int			time;
		j1Timer		timer;
	};

	vector<QueueTroop*>	Troop;

	// UI
	GuiItem* Button_Create_Gatherer;
	bool create_gatherer;
	GuiItem* Gatherer_image;

	//Progress Bar
	GuiItem* creation_TownHall_bar;

};
#endif // __TEST_1_H__
