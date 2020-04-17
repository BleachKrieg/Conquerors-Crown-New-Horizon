#ifndef __j1WAVESYSTEM_H__
#define __j1WAVESYSTEM_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Timer.h"

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
	iPoint spawn_point1;
	iPoint spawn_point2;
	iPoint spawn_point3;

};

#endif //__j1WAVESYSTEM_H__
