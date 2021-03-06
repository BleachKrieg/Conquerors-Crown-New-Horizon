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
#include "DynamicEnt.h"
#include "j1Fonts.h"
#include "j1Audio.h"
#include "MouseCursor.h"
#include "J1GroupMov.h"

GoldMine::GoldMine(int posx, int posy, uint amount) : StaticEnt(StaticEntType::GoldMine)
{
	name.create("gold_mine");
	position.x = posx - 64;
	position.y = posy - 64;
	body = 0;
	coll_range = 0;
	active = true;
	selectable = true;
	team = TeamType::NO_TYPE;
	no_light_mine = { 4,8,96,89 };
	out_of_material_mine = { 103,8,96,89 };
	has_limit = true;
	light_mine = { 4,104,96,89 };
	isSelected = false;
	extraction_limit = amount;
	max_resources = extraction_limit;
	createUI = false;
	mine_time = 0;
}

GoldMine::~GoldMine() {}

bool GoldMine::Start()
{
	mine_lights = LIGHTS_OFF;
	has_limit = true;
	to_delete = false;
	current_rect = &no_light_mine;
	if (extraction_limit >= 1999u)
		extraction_limit = 0u;
	pre_check = extraction_limit;
	return true;
}

bool GoldMine::Update(float dt)
{
	if (extraction_limit == 0)
	{
		current_rect = &out_of_material_mine;
	}
	else if (extraction_limit > 0 && mine_lights == LIGHTS_OFF)
	{
		current_rect = &no_light_mine;
	}
	else if (extraction_limit > 0 && mine_lights == LIGHTS_ON)
	{
		current_rect = &light_mine;
		if (mine_time >= 70) {
			SpatialAudio(11, App->audio->mine_gatherer, position.x, position.y);
			mine_time = 0;
		}
		mine_time++;
	}

	if (!App->mouse_cursor->on_resources && App->movement->player_selected != nullptr && App->movement->player_selected->GetDynEntType() == uint(DynamicEnt::DynamicEntityType::HUMAN_GATHERER))
	{
		iPoint m_pos;
		SDL_Rect r;
		App->input->GetMousePosition(m_pos.x, m_pos.y);
		m_pos = App->render->ScreenToWorld(m_pos.x, m_pos.y);
		r = *current_rect;
		r.x = position.x + 64;
		r.y = position.y + 64;
		r.w /= 2;
		r.h /= 2;
		if (m_pos.x > (r.x - r.w - 12) && m_pos.x < (r.x + r.w) && m_pos.y >(r.y - r.h - 12) && m_pos.y < (r.y + r.h - 24))
			App->mouse_cursor->on_resources = true;
	}

	

	App->render->Blit(App->entity->miscs, position.x, position.y, current_rect, 1.0F, 1.0F);

	SDL_Rect section;
	float 	bar_conversion = (float)50 / (float)max_resources;

	section.x = 0;
	section.y = 2;
	section.w = ((int)extraction_limit * bar_conversion);
	section.h = 2;

	if(extraction_limit< max_resources && extraction_limit != 0)
	App->render->Blit(App->entity->life_bar, (int)(position.x + (*current_rect).w / 3), (int)(position.y + (*current_rect).h), &section);

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

void GoldMine::DecreaseExtractionCount()
{
	if (extraction_limit > 0)extraction_limit--;
	else extraction_limit = 0;
}

uint GoldMine::GetPreCheck()
{
	return pre_check;
}

void GoldMine::DecreasePreCheck()
{
	if (pre_check > 0)pre_check--;
	else pre_check = 0;
}