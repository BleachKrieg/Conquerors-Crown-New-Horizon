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
	ST_BARRACK_UPGRADING,
	ST_BARRACK_AUTOMATIC,
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
	void GuiInput(GuiItem*);
	void QueueSwap();

public:
	// Animations
	Animation inconstruction;
	Animation finishedconst;
	SDL_Rect Construction;
	SDL_Rect Created;
    Barrack_states	actualState;
	
	// Stats
	int attackrange;
	int collrange;

	// Upgrade
	bool Barrack_Upgraded;

	// Queue
	float timer_queue;
	int troop_type;
	j1Timer start_queue;
	j1Timer timer_bar;
	bool time_bar_start;

	struct QueueTroop
	{
		int			time;
		int			type;
		GuiItem*	image;
		GuiItem*	bar;
		j1Timer		timer;
	};

	vector<QueueTroop*>	Troop;

	void CheckQueue();
	void ImageSelected();
	
	// UI
	GuiItem* Button_Create_Footman;
	GuiItem* Button_Create_Archer;
	GuiItem* Archer_image;
	GuiItem* Swordman_image;
	
	//Progress Bar
	GuiItem* creation_barrack_bar;

	//Upgrades
	float first_upgrade_time;
	j1Timer upgrade_timer;

	bool create_swordman;
	bool create_archer;

	// Queue positions
	iPoint pos0;
	iPoint pos1;
	iPoint pos2;
	iPoint pos3;
	iPoint pos4;
	iPoint pos5;

};
#endif // __TEST_1_H__
