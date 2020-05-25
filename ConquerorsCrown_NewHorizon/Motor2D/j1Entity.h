#ifndef __j1Entity_H__
#define __j1Entity_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "p2Point.h"
#include "p2Log.h"
#include "j1App.h"
#include <list>
#include "Animation.h"
#include "j1Audio.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include "FoWEntity.h"


using namespace std;

struct SDL_Texture;

struct TileSetEntity
{
	SDL_Rect GetAnimRect(int id) const;
	int tile_width;
	int tile_height;
	int firstgid;
	int num_tiles_width;
	int tex_width;
	p2SString Texname;
};

class j1Entity : public j1Module
{
public:
	enum class entityType
	{
		NO_TYPE,
		STATIC,
		DYNAMIC
	};

	enum class TeamType
	{
		NO_TYPE,
		PLAYER,
		IA,
	};


	// Constructor
	j1Entity(entityType type);

	// Destructor
	~j1Entity();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

	//Spatial Audio
	void SpatialAudio(int channel, int SFX, int posx, int posy);

	Animation* GetAnimation();


protected:
	Animation* current_animation = nullptr;


public: 	
	bool isSelected;
	bool selectable;
	bool selectable_buildings;
	fPoint position;
	bool to_delete;
	int body;
	int coll_range;
	fPoint speed;
	entityType type;
	TeamType	team;
	int		life_points;
	int		max_hp;
	vector<iPoint>	path;

	bool deployed = true;
	FoWEntity* visionEntity = nullptr;

	
};

#endif // __j1Entity_H__