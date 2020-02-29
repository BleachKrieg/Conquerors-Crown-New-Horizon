#ifndef __j1Entity_H__
#define __j1Entity_H__

#include "p2Point.h"
#include "p2Log.h"
#include "j1App.h"

class j1Entity : public j1Module
{
public:
	enum entityType
	{
		NO_TYPE,
		TEST_1
	};

	entityType type;

	// Constructor
	j1Entity(entityType type);

	// Destructor
	~j1Entity();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

public:
	// Size
	int width;
	int high;

	// Position
	iPoint position;

	// Speed
	float speed;

	// Blit
	bool blit = false;

	// Colliders
	Collider* collider = nullptr;
	bool to_delete;

};

#endif // __j1Entity_H__