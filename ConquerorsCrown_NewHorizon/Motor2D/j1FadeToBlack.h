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
	bool FadeToBlackVisualEffect(float time = 2.0f);

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
	int level = 0;

	SDL_Texture* screen_;
};

#endif //__J1FADETOBLACK_H__
