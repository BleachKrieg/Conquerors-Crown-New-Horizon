#ifndef __j1WAVESYSTEM_H__
#define __j1WAVESYSTEM_H__

#include "j1Module.h"
#include "Animation.h"
#include "j1Entity.h"
#include "p2Point.h"
#include "j1Timer.h"
#include "Troll_Enemy.h"
#include "Ogre_Enemy.h"
#include "Grunt_Enemy.h"


struct SpawnPoint
{
	iPoint		position;
	j1Entity*	target;
	fPoint		targetpos;
	vector<iPoint>	path;
	j1Entity*	bulding;
};

class j1WaveSystem : public j1Module {
public:

	j1WaveSystem();

	// Destructor
	virtual ~j1WaveSystem();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	bool CreateSpawnBuildings();

	void StartWave(int wave);
	void FinishWave();
	bool SpawnTroll(SpawnPoint* spawn);
	bool SpawnOgre(SpawnPoint* spawn);
	bool SpawnGrunt(SpawnPoint* spawn);

public:
	int current_wave;
	int next_wave;
	int spawn_counter;
	int max_waves;
	bool wave_ongoing;
	j1Timer wave_ended;
	j1Timer spawn_cooldown;
	int trolls;
	int ogres;
	int grunts;
	int troll_counter;
	int ogre_counter;
	int grunt_counter;
	int troll_value;
	int grunt_value;
	int ogre_value;
	int spawn_buildings;

	SpawnPoint* spawn1;
	SpawnPoint* spawn2;
	SpawnPoint* spawn3;
};

#endif //__j1WAVESYSTEM_H__
