#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "j1Entity.h"
#include "GoldMine.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "j1Fonts.h"
#include "j1Audio.h"

GoldMine::GoldMine(int posx, int posy) : StaticEnt(StaticEntType::GoldMine)
{
	name.create("gold_mine");
	position.x = posx - 64;
	position.y = posy - 64;
	body = 0;
	coll_range = 0;
	active = true;
	team = TeamType::NO_TYPE;
	no_light_mine.PushBack({ 4,8,96,89 }, 0.2, 0, 0, 0, 0);
	has_limit = true;
	light_mine.PushBack({ 4,104,96,89 }, 0.2, 0, 0, 0, 0);
	isSelected = false;
}

GoldMine::~GoldMine() {}

bool GoldMine::Start()
{
	mine_lights = LIGHTS_OFF;
	has_limit = true;
	extraction_limit = 20;
	iPoint pos = { (int)position.x, (int)position.y };
	pos = App->map->WorldToMap(pos.x, pos.y);
	iPoint tempPos = pos;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tempPos.x = pos.x + i;
			tempPos.y = pos.y + j;
			App->pathfinding->ChangeWalkability(tempPos, 1);
		}
	}

	return true;
}

bool GoldMine::Update(float dt)
{
	SDL_Rect* r;
	if (mine_lights == LIGHTS_OFF)
	{
		current_animation = &no_light_mine;
	}
	else if (mine_lights == LIGHTS_ON)
	{
		current_animation = &light_mine;
	}
	r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->miscs, position.x, position.y, r, 1.0F, 1.0F);

	return true;
}

bool GoldMine::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Postupdate_GoldMine", Profiler::Color::BurlyWood)

		return true;
}

bool GoldMine::CleanUp()
{

	return true;
}

uint GoldMine::GetExtractionLimit()
{
	return extraction_limit;
}