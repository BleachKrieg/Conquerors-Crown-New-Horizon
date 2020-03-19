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
		TEST_1,
		HUMAN_ARCHER,
		HUMAN_FOOTMAN,
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

public:



protected:
	uint life_points;

	int followpath;
	int collrange;
	int vision;
	bool move;
	SDL_RendererFlip orientation;
	iPoint origin, mouse, relative_target;
	p2DynArray<iPoint> path;
	list<j1Entity*> close_entity_list;
	list<j1Entity*> colliding_entity_list;

	
};

#endif // __j1Entity_H__