#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanBarracks.h"
#include "HumanTownHall.h"
#include "HumanUpgrade.h"
#include "HumanWall.h"
#include "HumanFootman.h"
#include "HumanArcher.h"
#include "HumanGatherer.h"
#include "Troll_Enemy.h"
#include "Ogre_Enemy.h"
#include "Grunt_Enemy.h"
#include "ResourceEntities.h"
#include "j1App.h"
#include <stdio.h>
#include "p2Log.h"
#include "GoldMine.h"
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
	trees_time = 10000;
	quarries_time = 10000;
	mines_time = 10000;

	foot_man_tex = App->tex->Load("Assets/textures/units/Human Sprites/human_footman.png");
	arch_man_tex = App->tex->Load("Assets/textures/units/Human Sprites/human_archer.png");
	foot_man_tex2 = App->tex->Load("Assets/textures/units/Human Sprites/human_footman_2.png");
	arch_man_tex2 = App->tex->Load("Assets/textures/units/Human Sprites/human_archer_2.png");
	gather_man_tex = App->tex->Load("Assets/textures/units/Human Sprites/human_gatherer.png");
	troll_tex = App->tex->Load("Assets/textures/units/Orc Sprites/orc_troll.png");
	ally_sel_tex = App->tex->Load("Assets/textures/units/selection_ally.png");
	enemy_sel_tex = App->tex->Load("Assets/textures/units/selection_enemy.png");
	ogre_tex = App->tex->Load("Assets/textures/units/Orc Sprites/orc_ogre.png");
	grunt_tex = App->tex->Load("Assets/textures/units/Orc Sprites/orc_grunt.png");
	
	LOG("Loading Dynamic Entities Animations");
	LoadAnimations("Assets/textures/units/Human Units Animations/archer_animations.tmx", archer_animations);
	LoadAnimations("Assets/textures/units/Human Units Animations/footman_animations.tmx", footman_animations);
	LoadAnimations("Assets/textures/units/Human Units Animations/gatherer_animations.tmx", gatherer_animations);
	LoadAnimations("Assets/textures/units/Orc Units Animations/troll_animations.tmx", troll_animations);
	LoadAnimations("Assets/textures/units/Orc Units Animations/ogre_animations.tmx", ogre_animations);
	LoadAnimations("Assets/textures/units/Orc Units Animations/grunt_animations.tmx", grunt_animations);

	building = App->tex->Load("Assets/textures/buildings/Human Buildings/human_buildings_summer.png");
	miscs = App->tex->Load("Assets/textures/misc/misc.png");

	max_audio_attacks = 0;
	timer.Start();

	return true;
}

bool j1EntityManager::CleanUp()
{
	
	j1Entity* it;

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->CleanUp();
		RELEASE(entities[i]);
	}

	entities.clear();

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("UpdateEntity", Profiler::Color::Bisque);

	Mix_AllocateChannels(20);

	if(timer.ReadMs() > 200)
	{
		max_audio_attacks = 0;
		timer.Start();
	}

	for (int i = 0; i < entities.size(); i++) {
		
		entities[i]->Update(dt);
	}
	
	return true;
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateEntity", Profiler::Color::Azure)


	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->to_delete == true)
		{
			DeleteEntity(i, entities[i]);
			i--;
		}
		else
		{
			entities[i]->PostUpdate();
		}
	}

	for (int i = 0; i < resources_ent.size(); i++) {
		if (resources_ent[i]->to_delete == true)
		{
			RELEASE(resources_ent[i]);
			resources_ent.erase(resources_ent.begin() + i);
			i--;
		}
	}

		return true;
}

j1Entity* j1EntityManager::CreateEntity(DynamicEnt::DynamicEntityType type, int posx, int posy)
{
	j1Entity* ret = nullptr;

	switch (type)
	{
	case DynamicEnt::DynamicEntityType::HUMAN_FOOTMAN: ret = new HumanFootman(posx, posy); player_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::HUMAN_ARCHER: ret = new HumanArcher(posx, posy); player_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::HUMAN_GATHERER: ret = new HumanGatherer(posx, posy); player_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::ENEMY_TROLL: ret = new TrollEnemy(posx, posy); ai_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::ENEMY_OGRE: ret = new OgreEnemy(posx, posy); ai_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::ENEMY_GRUNT: ret = new GruntEnemy(posx, posy); ai_dyn_ent.push_back(ret); break;
	}

	if (ret != nullptr)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	return ret;
}

j1Entity* j1EntityManager::CreateStaticEntity(StaticEnt::StaticEntType type, int posx, int posy, uint resource_type)
{
	j1Entity* ret = nullptr;

	switch (type)
	{
	case StaticEnt::StaticEntType::HumanBarracks: ret = new HumanBarracks(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::HumanTownHall: ret = new HumanTownHall(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::HumanUpgrade: ret = new Human_Upgrade(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::GoldMine: ret = new GoldMine(posx, posy); mines.push_back(ret); break;
	case StaticEnt::StaticEntType::HumanWall: ret = new Human_Wall(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::Resource: ret = new ResourceEntity(posx, posy, resource_type); resources_ent.push_back(ret); break;
	}

	if (ret != nullptr && type != StaticEnt::StaticEntType::Resource)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	else
	{
		resources_ent.back()->Start();
	}
	return ret;
}


bool j1EntityManager::DeleteAllEntities()
{

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->to_delete = true;
	}
	for (int i = 0; i < resources_ent.size(); ++i)
	{
		resources_ent[i]->to_delete = true;
		resources_ent[i]->CleanUp();
	}
	return true;
}

bool j1EntityManager::DeleteEntity(int id, j1Entity* entity)
{
	switch (entity->type)
	{
	case j1Entity::entityType::NO_TYPE:
		break;
	case j1Entity::entityType::DYNAMIC:
		switch (entity->team)
		{
		case j1Entity::TeamType::NO_TYPE:
			break;
		case j1Entity::TeamType::PLAYER:
			if (!player_dyn_ent.empty())
				player_dyn_ent.erase(std::find(player_dyn_ent.begin(), player_dyn_ent.end() + 1, entity));
			break;
		case j1Entity::TeamType::IA:
			if (!ai_dyn_ent.empty())
				ai_dyn_ent.erase(std::find(ai_dyn_ent.begin(), ai_dyn_ent.end() + 1, entity));
			break;
		}
		break;
	case j1Entity::entityType::STATIC:
		switch (entity->team)
		{
		case j1Entity::TeamType::NO_TYPE:
			if (!mines.empty())
				mines.erase(std::find(mines.begin(), mines.end() + 1, entity));
			break;
		case j1Entity::TeamType::PLAYER:
			if (!player_stat_ent.empty())
				player_stat_ent.erase(std::find(player_stat_ent.begin(), player_stat_ent.end() + 1, entity));
			break;
		case j1Entity::TeamType::IA:
			break;
		}
		break;
	}
	

	entities.erase(entities.begin() + id);
	entity->CleanUp();
	RELEASE(entity);



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
		set->loop = true;
		animations.push_back(set);
	}
}

bool j1EntityManager::IsSomethingSelected() 
{

	for (int i = 0; i < entities.size(); i++) {

		if (entities[i]->isSelected) return true;
	}
	return false;
}