#include <math.h>
#include "j1App.h"
#include "j1Scene.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Log.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

j1FadeToBlack::j1FadeToBlack()
{
	name.create("fadetoblack");
}

j1FadeToBlack::~j1FadeToBlack()
{}

// Load assets
bool j1FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	screen_ = App->tex->Load("textures/gui/LoadingScreen.png");
	return true;
}

// Update: draw background
bool j1FadeToBlack::Update(float dt)
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			App->scene->DeleteScene();
			App->scene->CreateScene(next_scene);
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	//App->render->Blit(screen_, 0, 0);

	return true;
}
bool j1FadeToBlack::PostUpdate(float dt) {

	// Finally render the black square with alpha on the screen
	if(current_step != fade_step::none)
		SDL_RenderFillRect(App->render->renderer, &screen);
	
	return true;
}

bool j1FadeToBlack::FadeToBlack(scenes scene, float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{

	}
	screen = { 0, 0, App->render->camera.w, App->render->camera.h };
	current_step = fade_step::fade_to_black;
	next_scene = scene;
	start_time = SDL_GetTicks();
	total_time = (Uint32)(time * 0.5f * 1000.0f);
	ret = true;

	return ret;
}