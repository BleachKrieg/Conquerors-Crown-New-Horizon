#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "Brofiler/Brofiler.h"
#include "j1Tutorial.h"
#include "j1FadeToBlack.h"

j1Tutorial::j1Tutorial() : j1Module()
{
	name.create("tutorial");
}


j1Tutorial::~j1Tutorial()
{}

bool j1Tutorial::Start()
{
	return true;
}

bool j1Tutorial::Update(float dt)
{
	BROFILER_CATEGORY("Tutorial", Profiler::Color::BlueViolet);
	//This is to change the scene
	
	return true;
}

bool j1Tutorial::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

		return true;
}

bool j1Tutorial::CleanUp()
{

	return true;
}
