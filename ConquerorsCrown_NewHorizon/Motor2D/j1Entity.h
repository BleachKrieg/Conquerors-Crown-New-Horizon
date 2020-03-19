#ifndef __j1Entity_H__
#define __j1Entity_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "p2Point.h"
#include "p2Log.h"
#include "j1App.h"
#include <list>
#include "Animation.h"

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

	entityType type;

	// Constructor
	j1Entity(entityType type);

	// Destructor
	~j1Entity();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	void LoadAnimations(const char* path);

protected:
	TileSetEntity TileSetData;
	Animation* current_animation = nullptr;
	list<Animation*> animations;

public: 	
	bool isSelected;
	bool selectable;
	fPoint position;
	bool to_delete;
	int body;
	fPoint speed;
	
	
};

#endif // __j1Entity_H__