#ifndef __j1EntityRequest_H__
#define __j1EntityRequest_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"

#include <list>

using namespace std;


class EntityRequest : public j1Module
{
public:
	// Constructor
	EntityRequest();

	// Destructor
	~EntityRequest();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	// Delete an entity

public:
	
};

#endif // __j1EntityManager_H__