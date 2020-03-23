#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "Test_3.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"

Test_3::Test_3(int posx, int posy) : StaticEnt( StaticEntType::TEST_3)
{
	name.create("test_1");
	position.x = posx;
	position.y = posy;
	vision = 80;
	selectable = false;
	isSelected = false;
	to_delete = false;
	finished = false;
	construction_time = 3000;
	timer = 0;
	// Load all animations
	inconstruction.PushBack({ 399,410,96,81 }, 0.2, 0, 0, 0, 0);
	finishedconst.PushBack({ 403,273,96,95 }, 0.2, 0, 0, 0, 0);
}

Test_3::~Test_3()
{}

bool Test_3::Start()
{
	timer = SDL_GetTicks();

	return true;
}

bool Test_3::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	//App->render->Blit(App->entity->test_1_graphics, position.x + current_animation->pivotx[current_animation->returnCurrentFrame()], position.y + current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)), 1.0f);
	
	if (finished)
	{
		if (isSelected == true)
		{
			App->render->DrawQuad({ (int)position.x - 3, (int)position.y - 3, 100, 100 }, 200, 0, 0, 200, false);

			if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			{
				App->requests->AddRequest(Petition::SPAWN, 1.f, SpawnTypes::SWORDMAN, { (int)position.x + 7, (int)position.y + 30 });
			}
		}

		// Finished Animation
		current_animation = &finishedconst;
	}
	else
	{
		// Construction Animation
		current_animation = &inconstruction;

		// Audio reproduced only once
		counter++;
		if (counter==1) {
			SpatialAudio(1);
			App->audio->PlayFx(1, App->audio->construction, 1);
		}
		
		//When construction finishes, pass in finished state and stop SFX
		if (SDL_GetTicks() >= construction_time + timer)
		{
			finished = true;
			Mix_HaltChannel(-1);
			counter = 0;
		}
	}


	if (App->scene->debug)
	{
		App->render->DrawCircle(position.x + 45, position.y +50, vision, 0, 0, 200);
		App->render->DrawQuad({ (int)position.x - 3, (int)position.y - 3, 100, 100 }, 200, 0, 0, 200, false);
	}

	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->building, (int)position.x, (int)position.y, r, 1.0f, 1.0f);
	return true;
}

bool Test_3::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

	return true;
}

bool Test_3::CleanUp()
{

	return true;
}

void Test_3::SpatialAudio(int channel) {
	
	Mix_HaltChannel(-1);
	
	float d;
	d = position.x * position.x + position.y * position.y;
	d = d / 500;
	int volume = (d * 255) / App->render->camera.w;
	if (volume < 0) {
		volume = 0;
	}
	if (volume > 255) {
		volume = 255;
	}

	float angle = 0;

	if (App->render->camera.y == position.y) {
		angle = atan(position.x);
	}
	else if (App->render->camera.y < position.y) {
		angle = atan(-position.x / position.y);
	}
	else {
		angle = atan(position.x / position.y);
	}
	angle = (angle * 57) + 360; //we add 360 cause of angle circumference

	Mix_SetPosition(channel, angle, volume);
}