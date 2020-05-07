#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Fonts.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "EntityRequest.h"
#include "j1Minimap.h"
#include "j1FadeToBlack.h"
#include "j1Tutorial.h"


j1Tutorial::j1Tutorial() : j1Module()
{
	name.create("tutorial");

}

// Destructor
j1Tutorial::~j1Tutorial()
{}

// Called before render is available
bool j1Tutorial::Awake(pugi::xml_node& config)
{
	bool ret = true;


	return ret;
}

// Called before the first frame
bool j1Tutorial::Start()
{
	bool ret = true;

	return ret;
}

// Called each loop iteration
bool j1Tutorial::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Scene", Profiler::Color::Snow);


	return true;
}

// Called each loop iteration
bool j1Tutorial::Update(float dt)
{
	BROFILER_CATEGORY("Update_Scene", Profiler::Color::Tomato);

	if (App->scene->tutorial) 
	{
		int a = 0;
	}

	return true;
}

// Called each loop iteration
bool j1Tutorial::PostUpdate(float dt)
{
	bool ret = true;


	return ret;
}

// Called before quitting
bool j1Tutorial::CleanUp()
{
	
	return true;
}