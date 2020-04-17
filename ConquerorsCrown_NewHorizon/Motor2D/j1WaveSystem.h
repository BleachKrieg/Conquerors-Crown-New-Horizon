#ifndef __j1WAVESYSTEM_H__
#define __j1WAVESYSTEM_H__

#include "j1Module.h"
#include "Animation.h"
#include "j1Entity.h"
#include "p2Point.h"
#include "j1Timer.h"
#include "Troll_Enemy.h"
#include "p2DynArray.h"


struct SpawnPoint
{
	iPoint		position;
	j1Entity*	target;
	fPoint		targetpos;
	p2DynArray<iPoint>	path;
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

	void StartWave(int wave);
	void FinishWave();

public:
	int current_wave;
	int next_wave;
	bool wave_ongoing;
	j1Timer wave_ended;

	SpawnPoint* spawn1;
	SpawnPoint* spawn2;
	SpawnPoint* spawn3;
};

#endif //__j1WAVESYSTEM_H__
