#ifndef __j1TUTORIAL_H__
#define __j1TUTORIAL_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "SDL/include/SDL.h"
#include "j1Timer.h"


struct SDL_Texture;
class j1Entity;
class DynamicEnt;
class StaticEnt;
class GuiItem;

class j1Tutorial : public j1Module
{
public:

	j1Tutorial();

	// Destructor
	virtual ~j1Tutorial();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

public:

};

#endif // __j1TUTORIAL_H__