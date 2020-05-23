#pragma once
#ifndef __StaticEnt_H__
#define __StaticEnt_H__

#include "p2Point.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Entity.h"
#include "j1Input.h"
#include "EntityRequest.h"
#include "j1Map.h"

class StaticEnt : public j1Entity
{
public:
	
	enum class StaticEntType
	{
		NO_TYPE,
		HumanBarracks,
		HumanTownHall,
		Resource,
		GoldMine,
		enemy_barrack
	};

	// Constructor
	StaticEnt(StaticEntType type);

	// Destructor
	~StaticEnt();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Get Tile on the map
	void GetTile();

public:

//	SDL_RendererFlip flip = SDL_FLIP_NONE;
	j1Entity* target;
	int vision;
	int damage;
	int armor;
	bool finished;
	bool preview;
	bool canbuild;
	iPoint map, world; // Preview mode
	int x, y; // Preview mode
	int construction_time;
	int time_FX_barracks;
	j1Timer timer;

	// UI
	bool createUI;

};

#endif // __j1Entity_H__