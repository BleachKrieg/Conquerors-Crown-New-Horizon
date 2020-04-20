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
	position.x = posx;
	position.y = posy;
	body = 40;
	active = true;
	team = TeamType::NO_TYPE;
	no_light_mine.PushBack({ 4,8,96,89 }, 0.2, 0, 0, 0, 0);
	light_mine.PushBack({ 4,104,96,89 }, 0.2, 0, 0, 0, 0);
}

GoldMine::~GoldMine() {}

bool GoldMine::Start()
{
	mine_lights = LIGHTS_OFF;
	iPoint pos = { (int)position.x, (int)position.y };
	pos = App->map->WorldToMap(pos.x, pos.y);
	iPoint tempPos = pos;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			tempPos.x = pos.x + i;
			tempPos.y = pos.y + j;
			App->entity->CreateStaticEntity(StaticEntType::Resource, tempPos.x, tempPos.y, 2u);
			App->pathfinding->ChangeWalkability(tempPos, 2);
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
	else if (mine_lights = LIGHTS_ON)
	{
		current_animation = &light_mine;
	}
	r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->miscs, world.x - 50, world.y - 50, r, 1.0F, 1.0F);

	iPoint pos = { (int)position.x, (int)position.y };
	pos = App->map->WorldToMap(pos.x, pos.y);
	iPoint tempPos = pos;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			tempPos.x = pos.x + i;
			tempPos.y = pos.y + j;
			App->render->DrawQuad(SDL_Rect{ int(world.x + (32 * i)), int(world.y + (32 * j)), 32, 32 }, 255, 255, 0, 255);
		}
	}

	return true;
}

bool GoldMine::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Postupdate_GoldMine", Profiler::Color::BurlyWood)

		return true;
}

bool GoldMine::CleanUp()
{
	iPoint pos = { (int)position.x, (int)position.y };
	pos = App->map->WorldToMap(pos.x, pos.y);
	iPoint tempPos = pos;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			tempPos.x = pos.x + i;
			tempPos.y = pos.y + j;
			App->pathfinding->ChangeWalkability(tempPos, 1);
		}
	}

	return true;
}