#ifndef __J1FADETOBLACK_H__
#define __J1FADETOBLACK_H__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	virtual ~j1FadeToBlack();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool FadeToBlack(scenes, float time = 2.0f);

	// Called before quitting
	bool CleanUp();

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	float normalized;
	SDL_Rect screen;
	int level = 0;
	scenes next_scene;
	SDL_Texture* screen_;
	int position;
	SDL_Texture* loadingIcon;
};

#endif //__J1FADETOBLACK_H__
