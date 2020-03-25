#include "j1EntityManager.h"
#include "j1Entity.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"

j1Entity::j1Entity(entityType type) : type(type)
{
	name.create("entity");
}

j1Entity::~j1Entity()
{}

bool j1Entity::Awake(pugi::xml_node& config)
{

	return true;
}

bool j1Entity::CleanUp()
{

	return true;
}

SDL_Rect TileSetEntity::GetAnimRect(int id) const
{
	int relative_id = id;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = ((rect.w) * (relative_id % num_tiles_width));
	rect.y = ((rect.h) * (relative_id / num_tiles_width));
	return rect;
}

void j1Entity::SpatialAudio(int channel) {

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