#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanBarracks.h"
#include "HumanTownHall.h"
#include "HumanFootman.h"
#include "HumanArcher.h"
#include "HumanGatherer.h"
#include "Troll_Enemy.h"
#include "j1App.h"
#include <stdio.h>
#include "p2Log.h"
#include "j1Textures.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager()
{
	name.create("entity_manager");
}

j1EntityManager::~j1EntityManager()
{}

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading EntityManager");

	return true;
}

bool j1EntityManager::Start()
{

	foot_man_tex = App->tex->Load("textures/units/Human Sprites/human_footman.png");
	arch_man_tex = App->tex->Load("textures/units/Human Sprites/human_archer.png");
	gather_man_tex = App->tex->Load("textures/units/Human Sprites/human_gatherer.png");
	troll_tex = App->tex->Load("textures/units/Orc Sprites/orc_troll.png");

	LOG("Loading Dynamic Entities Animations");
	LoadAnimations("textures/units/Human Units Animations/archer_animations.tmx", archer_animations);
	LoadAnimations("textures/units/Human Units Animations/footman_animations.tmx", footman_animations);
	LoadAnimations("textures/units/Human Units Animations/gatherer_animations.tmx", gatherer_animations);
	LoadAnimations("textures/units/Orc Units Animations/troll_animations.tmx", troll_animations);

	building = App->tex->Load("textures/buildings/Human Buildings/human_buildings_summer.png");
	max_audio_attacks = 0;
	timer.Start();
	return true;
}

bool j1EntityManager::CleanUp()
{
	
	list<j1Entity*>::iterator entities_list;
	j1Entity* it;

	for (entities_list = entities.begin(); entities_list != entities.end(); ++entities_list) {
		it = *entities_list;
		it->CleanUp();
		RELEASE(it);
	}

	entities.clear();

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("UpdateEntity", Profiler::Color::Bisque);
	if(timer.ReadMs() > 200)
	{
		max_audio_attacks = 0;
		timer.Start();
	}
	list<j1Entity*>::iterator entities_list;
	j1Entity* it;
	for (entities_list = entities.begin(); entities_list != entities.end(); ++entities_list) {
		it = *entities_list;
		it->Update(dt);
	}

	

	return true;
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateEntity", Profiler::Color::Azure)

		list<j1Entity*>::iterator entities_list;
	j1Entity* it;

	for (entities_list = entities.begin(); entities_list != entities.end(); ++entities_list) {
		it = *entities_list;
		if (it->to_delete == true)
		{
			DeleteEntity(entities_list, it);
		}
		else
		{
			it->PostUpdate();
		}
	}

		return true;
}

j1Entity* j1EntityManager::CreateEntity(DynamicEnt::DynamicEntityType type, int posx, int posy)
{
	j1Entity* ret = nullptr;

	switch (type)
	{
	case DynamicEnt::DynamicEntityType::HUMAN_FOOTMAN: ret = new HumanFootman(posx, posy); break;
	case DynamicEnt::DynamicEntityType::HUMAN_ARCHER: ret = new HumanArcher(posx, posy); break;
	case DynamicEnt::DynamicEntityType::HUMAN_GATHERER: ret = new HumanGatherer(posx, posy); break;
	case DynamicEnt::DynamicEntityType::ENEMY_TROLL: ret = new TrollEnemy(posx, posy); break;
	}

	if (ret != nullptr)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	return ret;
}

j1Entity* j1EntityManager::CreateStaticEntity(StaticEnt::StaticEntType type, int posx, int posy)
{
	j1Entity* ret = nullptr;

	switch (type)
	{
	case StaticEnt::StaticEntType::HumanBarracks: ret = new HumanBarracks(posx, posy); break;
	case StaticEnt::StaticEntType::HumanTownHall: ret = new HumanTownHall(posx, posy); break;
	}

	if (ret != nullptr)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	return ret;
}


bool j1EntityManager::DeleteAllEntities()
{

	list<j1Entity*>::iterator entities_list;
	j1Entity* it;

	for (entities_list = entities.begin(); entities_list != entities.end(); ++entities_list) {
		it = *entities_list;
		it->to_delete = true;
	}
	return true;
}

bool j1EntityManager::DeleteEntity(list<j1Entity*>::iterator entity_iterator, j1Entity* entity)
{
	entity->CleanUp();
	entities.erase(entity_iterator);

	return true;
}


void j1EntityManager::LoadAnimations(const char* path, list<Animation*>& animations) {
	pugi::xml_document	entity_file;
	pugi::xml_parse_result result = entity_file.load_file(path);
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());

	}
	TileSetData.firstgid = entity_file.child("map").child("tileset").attribute("firstgid").as_int();
	TileSetData.tile_width = entity_file.child("map").child("tileset").attribute("tilewidth").as_int();
	TileSetData.tile_height = entity_file.child("map").child("tileset").attribute("tileheight").as_int();
	TileSetData.tex_width = entity_file.child("map").child("tileset").child("image").attribute("width").as_int();
	TileSetData.Texname.create(entity_file.child("map").child("tileset").child("image").attribute("source").as_string());
	TileSetData.num_tiles_width = TileSetData.tex_width / TileSetData.tile_width;
	LOG("Tileset: %s", TileSetData.Texname.GetString());
	LOG("firstgid %d", TileSetData.firstgid);
	LOG("tile_width %d", TileSetData.tile_width);
	LOG("tile_height %d", TileSetData.tile_height);
	LOG("tex_width %d", TileSetData.tex_width);
	LOG("num_tiles_width %d", TileSetData.num_tiles_width);
	int i = 0;
	pugi::xml_node tile;
	pugi::xml_node frame;
	for (tile = entity_file.child("map").child("tileset").child("tile"); tile; tile = tile.next_sibling("tile")) {
		Animation* set = new Animation();
		for (frame = tile.child("animation").child("frame"); frame; frame = frame.next_sibling("frame")) {
			set->PushBack(TileSetData.GetAnimRect(frame.attribute("tileid").as_int()), (frame.attribute("duration").as_float()) / 2000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int(), 0, 0);
			LOG("Animation %d, %f, %d, %d", frame.attribute("tileid").as_int(), (frame.attribute("duration").as_float()) / 1000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int());
		}
		animations.push_back(set);
	}
}