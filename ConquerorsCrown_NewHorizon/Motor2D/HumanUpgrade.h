#ifndef __HUMANUPGRADE_H__
#define __HUMANUPGRADE_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "StaticEnt.h"
#include "EntityRequest.h"

enum UPGRADE_states
{
	ST_UPGRADE_PREVIEW,
	ST_UPGRADE_IN_CONSTRUCTION,
	ST_UPGRADE_FINISHED,
	ST_UPGRADE_UPGRADING,
	ST_UPGRADE_AUTOMATIC,
	ST_UPGRADE_DESTROYED
};

class Human_Upgrade : public StaticEnt
{
public:
	// Constructor
	Human_Upgrade(int posx, int posy);

	// Destructor
	~Human_Upgrade();

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
	void CreateUpgradeUI();
	void DeleteUpgradeUI();
	void GuiInput(GuiItem*);

public:
	
	// Animations
	Animation inconstruction;
	Animation finishedconst;
	SDL_Rect Construction;
	SDL_Rect Created;
	UPGRADE_states	actualState;


	// UI
	//Upgrade UI
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
	GuiItem* Button_Upgrade_Footman;
	GuiItem* Swordman_image;
	GuiItem* Swordman_stone_cost;
	GuiItem* Swordman_Text_stone;
	GuiItem* Swordman_gold_cost;
	GuiItem* Swordman_Text_Gold;

	//Knight UI
	GuiItem* Button_Upgrade_Knight;
	GuiItem* Knight_image;
	GuiItem* Knight_stone_cost;
	GuiItem* Knight_Text_stone;
	GuiItem* Knight_gold_cost;
	GuiItem* Knight_Text_Gold;

	//Progress Bar
	GuiItem* creation_barrack_bar;

	//Upgrades
	j1Timer upgrade_timer2;
	bool Swordman_Upgrade;
	bool Archer_Upgrade;
	bool Knight_Upgrade;
	bool time_bar_start;
	bool Upgrading_swordman;
	bool Upgrading_Archer;
	bool Upgrading_Knight;
	

	float first_upgrade_time;
	GuiItem* creation_Upgrade_bar;
	GuiItem* creation_Upgrade_bar_archer;
	GuiItem* creation_Upgrade_bar_Knight;

};
#endif // __UPGRADE_1_H__

