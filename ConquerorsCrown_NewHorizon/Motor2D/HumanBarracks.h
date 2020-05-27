#ifndef __HUMANBARRACKS_H__
#define __HUMANBARRACKS_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
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

	//FreeTile
	iPoint Searchtile(iPoint respawn);
public:
	// Animations
	Animation inconstruction;
	Animation finishedconst;
	SDL_Rect Construction;
	SDL_Rect Created;
    Barrack_states	actualState;
	
	// Stats
	int attackrange;

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
	//Upggrade UI
	GuiItem* Button_Upgrade;
	GuiItem* Barrack_image;
	GuiItem* Arrow_image;
	GuiItem* Upgrade_wood_cost;
	GuiItem* Upgrade_Text_wood;
	bool barrack_Ui_upgrade;

	//Archer UI
	GuiItem* Button_Create_Archer;
	GuiItem* Archer_image;
	GuiItem* Archer_stone_cost;
	GuiItem* Archer_Text_stone;
	GuiItem* Archer_gold_cost;
	GuiItem* Archer_Text_Gold;

	//Swordman UI
	GuiItem* Button_Create_Footman;
	GuiItem* Swordman_image;
	GuiItem* Swordman_stone_cost;
	GuiItem* Swordman_Text_stone;
	GuiItem* Swordman_gold_cost;
	GuiItem* Swordman_Text_Gold;
	
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

private:
	//Respawn positioning
	iPoint respawn;
	int randomrespawn;
	int randomrespawn2;

};
#endif // __TEST_1_H__
