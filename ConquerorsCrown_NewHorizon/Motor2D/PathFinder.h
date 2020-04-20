#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Entity.h"
#include "j1WaveSystem.h"

#include <list>
#include <vector>

#pragma region Structs

struct PathList;
struct SpawnPoint;

struct PathNode
{
	PathNode();
	PathNode(float g, float h, const iPoint& pos, PathNode* parent, bool isdiagonal = false);
	PathNode(const PathNode& node);
	uint FindWalkableAdjacents(PathList& list_to_fill);
	float Score() const;
	float CalculateF(const iPoint& destination);
	float g;
	float h;
	bool is_Diagonal;
	iPoint pos;
	PathNode* parent;
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{

	// Looks for a node in this list and returns it's list node or NULL
	const PathNode* Find(const iPoint& point) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	const PathNode* GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	vector<PathNode> list;

};
#pragma endregion

// TODO 1: Just take a few minutes to undertand this new class and think from where it comes

class PathFinder
{
public:
	PathFinder();
	~PathFinder();

	// Main function to request a path from A to B
	void PreparePath(const iPoint& origin, const iPoint& destination, j1Entity* requester = nullptr, SpawnPoint* callback = nullptr);
	bool IteratePath();


	// To request all tiles involved in the last generated path
	const vector<iPoint>* GetLastPath() const;
	
	vector<iPoint> last_path;

	bool Update();

	void SavePath(vector<iPoint*>* path);


	bool initSuccessful;
	bool pathCompleted;
	bool available;

private:

	PathList open;
	PathList close;

	iPoint origin;
	iPoint destination;

	
	int max_iterations;
	j1Entity* entity;
	SpawnPoint* callback;
	// we store the created path here
};

#endif // !__PATHFINDER_H__

