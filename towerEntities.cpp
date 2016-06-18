#include"towerEntities.h"
#include"collision.h"
#include<climits>
#include<cfloat>
#include<cmath>

#include<iostream>
using namespace std;

bool Vector2::operator==(const Vector2& rhs) {
	if (x == rhs.x && y == rhs.y) {
		return true;
	} else {
		return false;
	}
}
bool Vector2::operator!=(const Vector2& rhs) {
	return !(*this==rhs);
}

float distance2 (Vector2 posOne, Vector2 posTwo) {
	float xLen = posTwo.x-posOne.x;
	float yLen = posTwo.y-posOne.y;
	return sqrt(xLen*xLen+yLen*yLen);
}
Vector2 midpoint (Vector2 posOne, Vector2 posTwo) {
	Vector2 mid;
	mid.x = (posOne.x+posTwo.x)/2.0f;
	mid.y = (posOne.y+posTwo.y)/2.0f;
	return mid;
}
Vector2 midpoint (PathEdge edge) {
	return midpoint(edge.vertices[0],edge.vertices[1]);
}

bool Circle::contains (Vector2 pos) {
	Vector2 relative;
	relative.x = pos.x-center.x;
	relative.y = pos.y-center.y;
	if (relative.x*relative.x+relative.y+relative.y < radius*radius) {
		return true;
	} else {
		return false;
	}
}

BaseEntity::BaseEntity() : stats(STATS_SIZE) {}

Entity::Entity() : stats(STATS_SIZE) {
	script=NULL;
}
Entity& Entity::operator=(const Entity& rhs) {
	if (&rhs != this) {
		pos = rhs.pos;
		for (unsigned int i=0;i<STATS_SIZE;++i) {
			stats[i] = rhs.stats[i];
		}
		base = rhs.base;
		if (rhs.script) {
			script = rhs.script->copy();
			script->actor = this;
		}
	}
	return *this;
}
Entity::Entity(const Entity& original) : stats(STATS_SIZE) {
	*this = original;
}
Entity::~Entity() {
	delete script;
}
Entity Entity::create (float x, float y,BaseEntity* baseEnt,std::vector<Stat>modifiableList) {
	Entity ent;
	ent.pos.x = x;
	ent.pos.y = y;
	ent.base = baseEnt;
	for (unsigned int i = 0;i<modifiableList.size();++i) {
		ent.stats[modifiableList[i]] = baseEnt->stats[modifiableList[i]];
	}
	return ent;
}
Entity Entity::create (float x, float y,BaseEntity* baseEnt,unsigned int statRangeStart,unsigned int statRangeEnd) {
	Entity ent;
	ent.pos.x = x;
	ent.pos.y = y;
	ent.base = baseEnt;
	for (unsigned int i = statRangeStart;i<statRangeEnd;++i) {
		ent.stats[i] = baseEnt->stats[i];
	}
	return ent;
}

WayPath::WayPath() {
	next=0;
}
bool WayPath::isCurrent() {
	if (next < points.size()) {
		return true;
	}
	return false;
}
Vector2 WayPath::getCurrent() {
	return points[next];
}
bool WayPath::isNext() {
	if (next < points.size()-1) {
		return true;
	}
	return false;
}
Vector2 WayPath::getNext() {
	next++;
	return points[next];
}
void WayPath::pushPoint(Vector2 waypoint) {
	points.push_back(waypoint);
}

//special detection required for paths compared to regular box collision, the regular box collision
//includes two overlapping sides as a collision, but this does not neccessarily mean an overlap
//when it comes to paths.
bool PathRouter::pathOverlap (Vector2 startOne, Vector2 endOne, Vector2 startTwo, Vector2 endTwo) {
	if (startOne.x <= startTwo.x && startOne.y <= startTwo.y && endOne.x >= endTwo.x && endOne.y >= endTwo.y) {
		return true;
	}
	return false;
}
bool PathRouter::pathConnected (Vector2 startOne, Vector2 endOne, Vector2 startTwo, Vector2 endTwo) {
	if ((startTwo.x >= startOne.x && startTwo.x < endOne.x \
			&& (startTwo.y == endOne.y || endTwo.y == startOne.y)) \
			|| (startTwo.y >= startOne.y && startTwo.y < endOne.y \
			&& (startTwo.x == endOne.x || endTwo.x == startOne.x))) {
		return true;
	}
	return false;
}
PathRouter::PathRouter () {
}
PathRouter::PathRouter (ResourceManager::GameLevelWrapper newLevel) {
	int numTiles;
	ALLEGRO_MAP_LAYER* path = al_get_map_layer(newLevel,"Path");
	ALLEGRO_MAP_TILE** tiles = al_get_layer_tiles(newLevel,path,&numTiles);
	assert(numTiles>0);
	int levelWidth = al_get_map_width(newLevel);
	//generates a connected graph from the tiles of the path, each tile in it being a node of the graph.
	for (int tileIndex = 0; tileIndex < numTiles; ++tileIndex) {
		//Skip tiles that aren't the path
		if (al_get_tile_for_id(newLevel,al_get_single_tile_id(path,tileIndex%levelWidth,tileIndex/levelWidth))==0) {
			continue;
		}
		PathEdge edge;
		edge.vertices[0].x=(tileIndex%levelWidth)*al_get_tile_width(newLevel);
		edge.vertices[0].y=(tileIndex/levelWidth)*al_get_tile_height(newLevel);
		edge.vertices[1].x=((tileIndex%levelWidth)+1)*al_get_tile_width(newLevel);
		edge.vertices[1].y=((tileIndex/levelWidth)+1)*al_get_tile_height(newLevel);
		edges.push_back(edge);
	}
	edgeAdjacency.resize(edges.size());
	connectRoutes();
	al_free(tiles);
}
void PathRouter::connectRoutes() {
	for (unsigned int i=0;i<edges.size();++i) {
		for (unsigned int j=i+1;j<edges.size();++j) {
			if (pathConnected(edges[i].vertices[0],edges[i].vertices[1],
						edges[j].vertices[0],edges[j].vertices[1])) {
				edgeAdjacency[i].push_back(j);
				edgeAdjacency[j].push_back(i);
			}
		}
	}
}
void PathRouter::update(unsigned int edgeIndex, unsigned int costIndex, float newCost) {
	edges[edgeIndex].costs[costIndex] = newCost;
}
unsigned int PathRouter::addCost (unsigned int costsAdded) {
	assert(!edges.empty());
	unsigned int costIndex = edges[0].costs.size();
	for (unsigned int i=0;i<edges.size();++i) {
		edges[i].costs.resize(edges[i].costs.size()+costsAdded);
	}
	return costIndex;
}
unsigned int PathRouter::getEdgeIndex (Vector2 pos) {
	unsigned int edgeIndex = UINT_MAX;
	for (unsigned int i=0;i<edges.size();++i) {
		if (edges[i].vertices[0].x <= pos.x  \
				&& edges[i].vertices[0].y <= pos.y \
				&& edges[i].vertices[1].x >= pos.x \
				&& edges[i].vertices[1].y >= pos.y) {
			edgeIndex = i;
			break;
		}
	}
	return edgeIndex;
}
//shortestDist keeps track of the shortest distance throughout the entire process
//shortestPath is a stack returned containing the shortest distance in reverse order
void PathRouter::generatePathPoints(unsigned int startIndex, unsigned int endIndex, float* shortestDist,
		std::vector<unsigned int>* shortestPath, std::vector<float> costWeights, float dist,
		std::vector<unsigned int> pathPoints) {
	//skip already visited nodes
	for (unsigned int i=0;i<pathPoints.size();++i) {
		if (startIndex == pathPoints[i]) {
			return;
		}
	}
	//add node to visited
	pathPoints.push_back(startIndex);

	for (unsigned int i=0;i<edges[startIndex].costs.size();++i) {
		dist+=edges[startIndex].costs[i]*costWeights[i];
	}
	if (startIndex==endIndex) {
		if (dist < *shortestDist) {
			*shortestDist = dist;
			*shortestPath = pathPoints;
		}
		return;
	} else {
		for (unsigned int i=0;i<edgeAdjacency[startIndex].size();++i) {
			generatePathPoints(edgeAdjacency[startIndex][i],endIndex,shortestDist,
					shortestPath,costWeights,dist,pathPoints);
		}
	}
}
WayPath PathRouter::generatePath (Vector2 startPos, Vector2 endPos, std::vector<float> costWeights) {
	unsigned int edgeIndexes [2];
	cout<<"end ";
	cout<<endPos.x<<",";
	cout<<endPos.y<<endl;;
	float shortestDists [2] = {FLT_MAX,FLT_MAX};
	for (unsigned int i=0;i<edges.size();++i) {
		float dist = distance2(startPos,midpoint(edges[i]));
		if (dist < shortestDists[0]) {
			shortestDists[0]= dist;
			edgeIndexes[0] = i;
		}
		dist = distance2(endPos,midpoint(edges[i]));
		if (dist < shortestDists[1]) {
			shortestDists[1]= dist;
			edgeIndexes[1] = i;
		}
	}
	WayPath newPath;
	float shortestDist = FLT_MAX;
	Vector2 wayPoint;
	std::vector<unsigned int> shortestPath;
	generatePathPoints(edgeIndexes[0],edgeIndexes[1],&shortestDist,&shortestPath,costWeights);
	for (unsigned int i=0;i<shortestPath.size();++i) {
		edges[shortestPath[i]];
		wayPoint = midpoint(edges[shortestPath[i]]);
		newPath.pushPoint(wayPoint);
	}
	return newPath;
}
