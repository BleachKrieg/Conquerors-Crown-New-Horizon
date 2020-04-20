#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "p2Point.h"

#include "j1Timer.h"
#include "PathFinder.h"
#include <vector>

using namespace std;
#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

struct PathList;
struct SpawnPoint;

struct PathRequests
{
	iPoint origin;
	iPoint destination;
	j1Entity* requester;
	SpawnPoint* callback;
};

class j1PathFinding : public j1Module
{
public:

	j1PathFinding();

	// Destructor
	~j1PathFinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	uchar GetWalkability(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	iPoint InminentNeighbour(const iPoint& origin, const iPoint& destination) const;

	void ChangeWalkability(const iPoint& pos, const uchar& isWalkable);

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	int RequestPath(const iPoint& origin, const iPoint& destination, j1Entity* requester, SpawnPoint* callback = nullptr);

	bool Start() override;

	bool Update(float dt) override;

	j1Timer timer;

	// TODO 3: Create a vector of PathFinders to distribute the new paths. Make sure to iterate from all vector.
	vector<PathFinder*> pathfinderList;
	vector <PathRequests*> requestList;
private:

	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	uchar* map;

	bool requestPath;
};


#endif // __j1PATHFINDING_H__