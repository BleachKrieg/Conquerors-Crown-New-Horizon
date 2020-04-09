#include "PathFinder.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Pathfinding.h"
#include "j1Entity.h"


PathFinder::PathFinder() : last_path(DEFAULT_PATH_LENGTH), initSuccessful(false), pathCompleted(false),max_iterations(50),available(true)
{
	LOG("PathFinder created");
}

PathFinder::~PathFinder()
{
}



void PathFinder::PreparePath(const iPoint& origin, const iPoint& destination, j1Entity* requester)
{
	// Add the origin tile to open
	if (open.GetNodeLowestScore() == NULL)
		open.list.add(PathNode(0, origin.DistanceTo(destination), origin, nullptr));

	uint iterations = 0;
	this->entity = requester;
	this->origin = origin;
	this->destination = destination;
	initSuccessful = true;
	available = false;
	
}

bool PathFinder::IteratePath()
{
	//TODO 2: This function won't need a loop inside anymore, we are controlling this loop outside
	bool ret = true;

	PathNode* node = new PathNode(open.GetNodeLowestScore()->data);
	close.list.add(*node);
	open.list.del(open.Find(node->pos));


	if (node->pos == destination) {
		const PathNode* iterator = node;

		last_path.Clear();
		// Backtrack to create the final path
		for (iterator; iterator->pos != origin; iterator = iterator->parent)
		{
			last_path.PushBack(iterator->pos);
		}

		last_path.PushBack(origin);

		// Use the Pathnode::parent and Flip() the path when you are finish
		last_path.Flip();
		pathCompleted = true;
		initSuccessful = false;
		available = true;
		open.list.clear();
		close.list.clear();
		SavePath(&entity->path);
		RELEASE(node);

		return false;
	}


	PathList adjacentNodes;
	uint numNodes = node->FindWalkableAdjacents(adjacentNodes);


	for (uint i = 0; i < numNodes; i++)
	{
		// ignore nodes in the closed list
		if (close.Find(adjacentNodes.list[i].pos) == NULL) {
			// If it is NOT found, calculate its F and add it to the open list
			if (open.Find(adjacentNodes.list[i].pos) == NULL) {
				adjacentNodes.list[i].CalculateF(destination);
				open.list.add(adjacentNodes.list[i]);
			}
			// If it is already in the open list, check if it is a better path (compare G)
			else {
				if (adjacentNodes.list[i].g < open.Find(adjacentNodes.list[i].pos)->data.g) {
					// If it is a better path, Update the parent
					adjacentNodes.list[i].CalculateF(destination);
					open.list.del(open.Find(adjacentNodes.list[i].pos));
					open.list.add(adjacentNodes.list[i]);
				}
			}
		}
	}
	return ret;
}



// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* PathFinder::GetLastPath() const
{
	return &last_path;
}

bool PathFinder::Update()
{	
	//TODO 2: Make a loop to take control on how many times the function "IteratePath" should be called in one frame
	bool ret = true;
	for (int i = 0; i < max_iterations && ret; i++)
	{
		 ret = IteratePath();
	}

	return ret;
}


#pragma region PathList



// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

void PathFinder::SavePath(p2DynArray<iPoint>* path)
{
	const p2DynArray<iPoint>* last_path = GetLastPath();
	path->Clear();
	for (uint i = 0; i < last_path->Count(); ++i)
	{
		path->PushBack({ last_path->At(i)->x, last_path->At(i)->y });
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
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, true));

	// south
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, true));

	// east
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, true));

	// west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, true));

	return list_to_fill.list.count();
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

