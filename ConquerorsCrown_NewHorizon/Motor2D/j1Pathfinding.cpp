#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "j1Entity.h"


j1PathFinding::j1PathFinding() : j1Module(), map(NULL),width(0), height(0),requestPath(false)
{
	name.create("pathfinding");


}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	for (int i = 0; i < pathfinderList.size(); i++)
	{
	pathfinderList[i]->last_path.clear();
	}
	pathfinderList.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

uchar j1PathFinding::GetWalkability(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t;
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	if (t == (uchar)2)
	{
		return false;
	}
	return t;
}

iPoint j1PathFinding::InminentNeighbour(const iPoint& origin, const iPoint& destination) const
{
	iPoint ret = destination;
	for (int max = 0; max < 100; ++max)
	{
		for (int i = (-1 - max); i < (2 + max); ++i)
		{
			for (int j = (-1 - max); j < (2 + max); ++j)
			{
				if (map[((destination.y + j) * width) + destination.x + i] == 1)
				{
					if (origin.DistanceTo(iPoint(destination.x + i, destination.y + j)) < origin.DistanceTo(ret))
					{
						ret = iPoint(destination.x + i, destination.y + j);
					}
				}
			}
		}
		if (ret != destination)
		{
			return ret;
		}
	}

	return origin;
}

void j1PathFinding::ChangeWalkability(const iPoint& pos, const uchar& isWalkable)
{
	if (CheckBoundaries(pos) && map != NULL)
	{
		map[(pos.y * width) + pos.x] = isWalkable;
	}
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}


// TODO 3: Remember, now we want to iterate from all PathFinders and check if it's available.

int j1PathFinding::RequestPath(const iPoint& origin, const iPoint& destination, j1Entity* requester, SpawnPoint* callback)
{
	LOG("Requesting a path...");
	iPoint dest = destination;

	if (GetWalkability(origin) == 0 || GetWalkability(destination) == 0) {
		if(requester != NULL)
		requester->path.clear();
		return -1;
	}
	else if (GetWalkability(destination) == 2)
	{
		dest = InminentNeighbour(origin, destination);
	}
	requestPath = true;
	PathRequests* NewRequest = new PathRequests;
	NewRequest->origin = origin;
	NewRequest->destination = dest;
	NewRequest->requester = requester;
	NewRequest->callback = callback;
	requestList.push_back(NewRequest);
	return 0;

//	return -1;
}

bool j1PathFinding::Start()
{
	//TODO 3: Add PathFinder to the vector.

	PathFinder* pathfinder01 = new PathFinder;
	PathFinder* pathfinder02 = new PathFinder;
	pathfinderList.push_back(pathfinder01);
	pathfinderList.push_back(pathfinder02);
	PathFinder* pathfinder03= new PathFinder;
	pathfinderList.push_back(pathfinder03);

	return true;
}



bool j1PathFinding::Update(float dt)
{
	

	for (int i = 0; i < pathfinderList.size(); i++)
	{
		if (pathfinderList[i]->available && !requestList.empty()) {
			pathfinderList[i]->PreparePath(requestList[0]->origin, requestList[0]->destination, requestList[0]->requester, requestList[0]->callback);
			requestList.erase(requestList.begin());
			LOG("Requested succeed");
		}

		if (!pathfinderList[i]->available)
			 pathfinderList[i]->Update();
	}
		

	return true;
}


// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------

