#ifndef __MOUSE_CURSOR_H__
#define __MOUSE_CURSOR_H__

#include "j1Module.h"
#include "j1Textures.h"
#include "SDL/include/SDL_rect.h"

class MouseCursor : public j1Module
{
public:
	MouseCursor();

	~MouseCursor() {}

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:
	SDL_Texture* cursor_tex = nullptr;
	SDL_Rect resource_cursor;
	SDL_Rect attack_cursor;
	SDL_Rect prohibited_cursor;
	SDL_Rect normal_cursor;

public:
	bool on_resources;
	bool to_attack;
};

#endif // !__MOUSE_CURSOR_H__