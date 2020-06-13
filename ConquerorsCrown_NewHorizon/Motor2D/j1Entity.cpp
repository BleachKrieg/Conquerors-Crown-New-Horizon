#include "j1EntityManager.h"
#include "j1Entity.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Audio.h"
#include "j1Timer.h"

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


bool j1Entity::Load(pugi::xml_node& data) {
	return true;
}

bool j1Entity::Save(pugi::xml_node& data) const {
	data.append_child("type");
	data.append_child("position");
	switch (type)
	{
	case  entityType::NO_TYPE:
		data.child("type").append_attribute("movement") = "none";
		data.child("type").append_attribute("name") = name.GetString();
		break;
	case  entityType::STATIC:
		data.child("type").append_attribute("movement") = "static";
		data.child("type").append_attribute("name") = name.GetString();
		if (name == "gold_mine")
		{
			for (int i = 0; i < App->entity->mines.size(); ++i) {
				if (App->entity->mines[i] == this)
					data.child("type").append_attribute("amount_left") = App->entity->mines[i]->GetExtractionLimit();
			}
		}
		break;
	case  entityType::DYNAMIC:
		data.child("type").append_attribute("movement") = "dynamic";
		data.child("type").append_attribute("name") = name.GetString();
		break;
	}
		
	
	data.child("position").append_attribute("pos_x") = position.x;
	data.child("position").append_attribute("pos_y") = position.y;
	


	
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

void j1Entity::SpatialAudio(int channel, int SFX, int posx, int posy) {

	Mix_Playing(channel);
	Mix_HaltChannel(channel);
	iPoint center_camera = { -App->render->camera.x + App->render->camera.w / 2, -App->render->camera.y + App->render->camera.h / 2 };
	//App->render->DrawQuad(SDL_Rect{ center_camera.x, center_camera.y, 2,2 }, 255, 255, 255, 255);
	iPoint provisional_distance = { posx - center_camera.x, posy - center_camera.y };
	int normalize = (provisional_distance.x * provisional_distance.x + provisional_distance.y * provisional_distance.y) / 400;
	App->entity->volume = (normalize * 255) / (App->render->camera.w);
	if (App->entity->volume < 0) {
		App->entity->volume = 0;
	}
	if (App->entity->volume > 255) {
		App->entity->volume = 255;
	}

	float angle = 0;

	if (center_camera.y == posy) {
		angle = atan(provisional_distance.x);
	}
	else if (center_camera.y < posy) {
		angle = atan(provisional_distance.x / provisional_distance.y);
	}
	else {
		angle = atan(-provisional_distance.x / provisional_distance.y);
	}
	angle = (angle * 57) + 360; //we add 360 cause of angle circumference
	
	Mix_SetPosition(channel, angle, App->entity->volume);
	App->audio->PlayFx(channel, SFX, 0);
}

Animation* j1Entity::GetAnimation()
{
	return current_animation;
}