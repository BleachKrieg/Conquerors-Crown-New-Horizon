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
	no_light_mine.PushBack({ 4,8,96,89 }, 0.2, 0, 0, 0, 0);
	out_of_material_mine.PushBack({ 103,8,96,89 }, 0.2, 0, 0, 0, 0);
	has_limit = true;
	light_mine.PushBack({ 4,104,96,89 }, 0.2, 0, 0, 0, 0);
	isSelected = false;
	extraction_limit = amount;
	createUI = false;
}

GoldMine::~GoldMine() {}

bool GoldMine::Start()
{
	mine_lights = LIGHTS_OFF;
	has_limit = true;
	to_delete = false;
	if (extraction_limit >= 1999u)
		extraction_limit = 0u;
	pre_check = extraction_limit;
	return true;
}

bool GoldMine::Update(float dt)
{
	SDL_Rect* r;
	if (extraction_limit == 0)
	{
		current_animation = &out_of_material_mine;
	}
	else if (extraction_limit > 0 && mine_lights == LIGHTS_OFF)
	{
		current_animation = &no_light_mine;
	}
	else if (extraction_limit > 0 && mine_lights == LIGHTS_ON)
	{
		current_animation = &light_mine;
	}

	if (!App->mouse_cursor->on_resources && App->movement->player_selected != nullptr && App->movement->player_selected->GetDynEntType() == uint(DynamicEnt::DynamicEntityType::HUMAN_GATHERER))
	{
		iPoint m_pos;
		SDL_Rect r;
		App->input->GetMousePosition(m_pos.x, m_pos.y);
		m_pos = App->render->ScreenToWorld(m_pos.x, m_pos.y);
		r = GetAnimation()->GetCurrentSize();
		r.x = position.x + 64;
		r.y = position.y + 64;
		r.w /= 2;
		r.h /= 2;
		if (m_pos.x > (r.x - r.w - 12) && m_pos.x < (r.x + r.w) && m_pos.y >(r.y - r.h - 12) && m_pos.y < (r.y + r.h - 24))
			App->mouse_cursor->on_resources = true;
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