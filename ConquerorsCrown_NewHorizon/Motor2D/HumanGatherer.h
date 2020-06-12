#ifndef __HUMANGATHERER_H__
#define __HUMANGATHERER_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "GoldMine.h"
#include "DynamicEnt.h"

class HumanGatherer : public DynamicEnt
{
public:

	enum class WORK_STATE
	{
		NONE,
		WORKING,
		GO_TO_WORK,
		GO_TO_TOWNHALL,
	};

	// Constructor
	HumanGatherer(int posx, int posy);

	// Destructor
	~HumanGatherer();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all the updates
	bool PostUpdate(float dt);

	bool CleanUp();

	void CheckTownHall();

	void AssignTownHall();

private:
	j1Entity* town_hall;
	j1Entity* work_space;
	GoldMine* work_mine_space;

	p2SString work_name;

	uint start_time;

	uint work_time;

	uint inv_size;

	WORK_STATE work_state;

	j1PerfTimer timer;

	int chop_time;
	int quarry_time;

	bool to_blit;

};


#endif // !__HUMANGATHERER_H__
