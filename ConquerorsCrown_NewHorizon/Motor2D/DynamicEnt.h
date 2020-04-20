#pragma once
#ifndef __DynamicEnt_H__
#define __DynamicEnt_H__

#include "p2Point.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Entity.h"
#include "SDL/include/SDL_render.h"

struct SDL_Texture;

class DynamicEnt : public j1Entity
{
public:

	enum class DynamicEntityType
	{
		NO_TYPE,
		HUMAN_ARCHER,
		HUMAN_FOOTMAN,
		HUMAN_GATHERER,
		ENEMY_TROLL,
	};
	enum class DynamicState
	{
		IDLE,
		UP,
		DOWN,
		HORIZONTAL,
		DIAGONAL_UP,
		DIAGONAL_DOWN,
		INTERACTING,
		DYING
	};
	
	// Constructor
	DynamicEnt(DynamicEntityType type);

	// Destructor
	~DynamicEnt();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	void CheckCollisions(fPoint* speed);

	void Movement();

	void SaveNeighbours(list<j1Entity*>*, list<j1Entity*>*);

	void OrderPath(DynamicEntityType type);

	void AttackTarget(DynamicEntityType type);

	void GathererGoTos();

	void Death(DynamicEntityType entity_type);
public:
	DynamicEntityType entity_type;

protected:

	int					followpath;
	uint				vision;
	uint				move;
	SDL_RendererFlip	orientation;
	iPoint				origin, mouse, relative_target;
	list<j1Entity*>		close_entity_list;
	list<j1Entity*>		colliding_entity_list;
	// States ------------------------
	DynamicState		state;
	// Animations ------------------------
	Animation moving_up;
	Animation moving_diagonal_up;
	Animation moving_right;
	Animation moving_diagonal_down;
	Animation moving_down;
	Animation attacking_up;
	Animation attacking_diagonal_up;
	Animation attacking_right;
	Animation attacking_diagonal_down;
	Animation attacking_down;
	Animation death_up;
	Animation death_down;
	// -----------------------------------
	// Attack Variables ------------------
	j1Entity*	target_entity;
	uint		attack_vision;
	uint		attack_range;
	uint		attack_damage;
	bool		following_target;
	bool		player_order;
	bool		can_attack;
	uint		time_attack;
	// -----------------------------------

	// Time counter ----------------------
	j1PerfTimer	timer;
	uint		current_time;

private:
	bool		change_direction;
	int			death_counter;
};

#endif // __j1Entity_H__