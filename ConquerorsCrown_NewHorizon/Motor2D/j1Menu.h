#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
#include "Animation.h"

#define COORDS(a) a+3000 

struct SDL_Texture;
class j1Entity;
class DynamicEnt;
class StaticEnt;;


class GuiItem;

class j1Menu : public j1Module
{
public:

	j1Menu();

	// Destructor
	virtual ~j1Menu();

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

	bool Load(pugi::xml_node&);

private:

public:

	SDL_Texture* Hello;
	SDL_Texture* debug_tex;
	GuiItem* buttonNewGame;
	GuiItem* buttonLoadGame;
	GuiItem* buttonOptions;
	GuiItem* buttonExit;
	bool debug;
};

#endif // __j1MENU_H__
