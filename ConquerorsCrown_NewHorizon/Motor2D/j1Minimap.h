#ifndef _j1MINIMAP_H
#define _j1MINIMAP_H
#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL.h"
#include "j1PerfTimer.h"

enum class Corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

class j1Minimap : public j1Module {
public:
	j1Minimap();
	~j1Minimap();

	bool Start();
	bool Awake(pugi::xml_node& config);
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);


	bool CreateMinimap();
	iPoint WorldToMinimap(int x, int y);
	iPoint ScreenToMinimapToWorld(int x, int y);
	bool CleanUp();


public:
	iPoint position;
	int width;
	int height;
	SDL_Texture* texture;
	bool visible;
	bool input;
	float scale;

private:
	int map_width;
	int map_height;
	int margin;
	Corner corner;
	SDL_Rect minimap_test_rect;
	j1PerfTimer	ptimer;
	SDL_Texture* minimap_entities;
};

#endif // !_j1MINIMAP_H


