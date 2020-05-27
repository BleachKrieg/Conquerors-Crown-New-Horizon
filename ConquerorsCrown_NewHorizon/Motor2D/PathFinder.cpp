#include "PathFinder.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Pathfinding.h"
#include "j1Entity.h"
#include "j1Scene.h"


PathFinder::PathFinder() : initSuccessful(false), pathCompleted(false),max_iterations(150),available(true), max_frames(0)
{
	LOG("PathFinder created");
}

PathFinder::~PathFinder()
{
}



void PathFinder::PreparePath(const iPoint& origin, const iPoint& destination, j1Entity* requester, SpawnPoint* callback)
{
	// Add the origin tile to open
	if (open.GetNodeLowestScore() == nullptr)
		open.list.push_back(PathNode(0, origin.DistanceTo(destination), origin, nullptr));

	uint iterations = 0;
	this->entity = requester;
	this->origin = origin;
	this->destination = destination;
	this->callback = callback;
	initSuccessful = true;
	available = false;
	
}

bool PathFinder::IteratePath()
{
	//TODO 2: This function won't need a loop inside anymore, we are controlling this loop outside
	bool ret = true;
	if (open.GetNodeLowestScore() != NULL)
	{

		PathNode* node = new PathNode(*open.GetNodeLowestScore());
		close.list.push_back(*node);

		for (int i = 0; i < open.list.size(); i++)
		{
			if (open.list[i].pos == node->pos)
			{
				open.list.erase(open.list.begin() + i);
			}

		}
		
		//	player_stat_ent.erase(std::find(player_stat_ent.begin(), player_stat_ent.end() + 1, entity));
		bool stop = false;
		if (entity != NULL && (App->scene->current_scene == scenes::ingame || App->scene->current_scene == scenes::tutorial))
			if (entity->life_points <= 0)
				stop = true;
		if (node->pos == destination || stop) {
			const PathNode* iterator = node;

			last_path.clear();
			// Backtrack to create the final path
			for (iterator; iterator->pos != origin; iterator = iterator->parent)
			{
				last_path.push_back(iterator->pos);
			}

			last_path.push_back(origin);

			// Use the Pathnode::parent and Flip() the path when you are finish
			std::reverse(last_path.begin(), last_path.end());
			pathCompleted = true;
			initSuccessful = false;
			available = true;
			open.list.clear();
			close.list.clear();
			if (entity != NULL && (App->scene->current_scene == scenes::ingame || App->scene->current_scene == scenes::tutorial))
			{
				if (entity->life_points <= 0)
				{
					entity = nullptr;
				}
				else {
					SavePath(&entity->path);
				}
			}
				
			if (callback != nullptr)
				SavePath(&callback->path);
			RELEASE(node);

			return false;
		}


		PathList adjacentNodes;
		uint numNodes = node->FindWalkableAdjacents(adjacentNodes);


		for (uint i = 0; i < numNodes; i++)
		{
			// ignore nodes in the closed list
			if (close.Find(adjacentNodes.list[i].pos) == nullptr) {
				// If it is NOT found, calculate its F and add it to the open list
				if (open.Find(adjacentNodes.list[i].pos) == nullptr) {
					adjacentNodes.list[i].CalculateF(destination);
					open.list.push_back(adjacentNodes.list[i]);
				}
				// If it is already in the open list, check if it is a better path (compare G)
				else {
					if (adjacentNodes.list[i].g < open.Find(adjacentNodes.list[i].pos)->g) {
						// If it is a better path, Update the parent
						adjacentNodes.list[i].CalculateF(destination);
						//open.list.erase(std::find(open.list.begin(), open.list.end() + 1, adjacentNodes.list[i]));
						for (int j = 0; j < open.list.size(); j++)
						{
							if (open.list[j].pos == adjacentNodes.list[i].pos)
							{
								open.list.erase(open.list.begin() + j);
							}
						}
						open.list.push_back(adjacentNodes.list[i]);
					}
				}
			}
		}
	}
	return ret;
}



// To request all tiles involved in the last generated path
const vector<iPoint>* PathFinder::GetLastPath() const
{
	return &last_path;
}

bool PathFinder::Update()
{	
	//TODO 2: Make a loop to take control on how many times the function "IteratePath" should be called in one frame
	bool ret = true;
	
	max_frames++;
	if (max_frames > 120)
	{
		max_frames = 0;
		pathCompleted = true;
		initSuccessful = false;
		available = true;
		open.list.clear();
		close.list.clear();
		entity = nullptr;
		callback = nullptr;
	}
	else {
		for (int i = 0; i < max_iterations && ret; i++)
		{
			ret = IteratePath();
		}
	}
	

	return ret;
}


#pragma region PathList



// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const PathNode* PathList::Find(const iPoint& point) const
{
	const PathNode* ret = nullptr;

	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].pos == point)
		{
			ret = &list[i];
			return ret;
		}
			
	}
	return ret;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
const PathNode* PathList::GetNodeLowestScore() const
{

	int min = 65535;
	const PathNode* ret = nullptr;


	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].Score() < min)
		{
			min = list[i].Score();
			ret = &list[i];
		}
	}
	
	return ret;
}

void PathFinder::SavePath(vector<iPoint>* path)
{
	if (path != nullptr && (App->scene->current_scene == scenes::ingame || App->scene->current_scene == scenes::tutorial) )
	{
		const vector<iPoint>* last_path = GetLastPath();
		path->clear();
		for (uint i = 0; i < last_path->size(); ++i)
		{
			iPoint point(last_path->at(i).x, last_path->at(i).y);

			path->push_back(point);
		}
		LOG("saving path");
	}
	
}
#pragma endregion

#pragma region PathNode



// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL), is_Diagonal(false)
{}

PathNode::PathNode(float g, float h, const iPoint& pos, PathNode* parent, bool isdiagonal) : g(g), h(h), pos(pos), parent(parent), is_Diagonal(isdiagonal)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent), is_Diagonal(node.parent)
{}


uint PathNode::FindWalkableAdjacents(PathList& list_to_fill)
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this, true));

	// south
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this, true));

	// east
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this, true));

	// west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this, true));

	return list_to_fill.list.size();
}


float PathNode::Score() const
{
	return g + h;
}


float PathNode::CalculateF(const iPoint& destination)
{
	if (!is_Diagonal)
	{
		g = parent->g + 1;
	}
	else {
		g = parent->g + 1.5;
	}
	h = pos.DistanceTo(destination);

	return g + h;
}




#pragma endregion

