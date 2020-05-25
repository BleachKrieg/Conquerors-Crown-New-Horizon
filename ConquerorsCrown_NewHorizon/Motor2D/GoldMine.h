#ifndef __GOLDMINE_H__
#define __GOLDMINE_H__

#include "j1Module.h"
#include "p2Point.h"
#include "StaticEnt.h"

enum MINE_LIGHTS
{
	LIGHTS_ON,
	LIGHTS_OFF,
};

class GoldMine : public StaticEnt
{
public:
	// Constructor
	GoldMine(int posx, int posy);

	// Destructor
	~GoldMine();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	bool PostUpdate(float dt);

	bool CleanUp();

	MINE_LIGHTS mine_lights;

	bool has_limit;

	uint GetExtractionLimit();

private:
	Animation no_light_mine;
	Animation light_mine;
	uint extraction_limit;
};

#endif // !__GOLDMINE_H__