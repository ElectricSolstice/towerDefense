#ifndef TOWER_ENTITIES
#define TOWER_ENTITIES

#include<vector>
#include"resourceManager.h"
#include"gameLevel.h"
#include<memory>
class Vector2 {
	public:
		float x;
		float y;
		bool operator==(const Vector2& rhs);
		bool operator!=(const Vector2& rhs);
};

class Circle {
	public:
		Vector2 center;
		float radius;
		bool contains(Vector2 pos);
};

float distance2 (Vector2 posOne, Vector2 posTwo);
Vector2 midpoint (Vector2 posOne, Vector2 posTwo);

class Entity;
class AIScript {
	public:
		virtual ~AIScript();
		virtual AIScript* copy ()=0;
		template<class T> static inline AIScript* defaultCopy (T* self) {
			return (AIScript*) new T(*self);
		}
		virtual void onClick ();
		virtual void onPoint ();
		virtual void onStep ();
		Entity* actor;
};


//Due to design of base and modifiable stats in an entity, stats listed first are
//modifiable stats that are stored on a per entity basis to make initialization of entities easier.
enum Stat {
	STAT_HP,
	STAT_SPD, STAT_STR,
	STATS_SIZE
};

//Base is data shared amongst all entities of the same type
struct BaseEntity {
	BaseEntity();
	std::vector<int> stats;
	ResourceManager::ImageWrapper img;
};

class Entity {
	public:
		Entity();
		Entity(const Entity& original);
		Entity& operator=(const Entity& rhs);
		~Entity();
		static Entity create (float posX, float posY, BaseEntity* type, std::vector<Stat> modifiableList);
		static Entity create (float posX, float posY, BaseEntity* type,
				unsigned int statRangeStart=STAT_HP,unsigned int statRangeEnd=STAT_HP+1);
		Vector2 pos;
		std::vector<int> stats;
		BaseEntity* base;
		AIScript* script;
};

class WayPath {
	public:
		WayPath();
		bool isCurrent();
		Vector2 getCurrent();
		bool isNext();
		Vector2 getNext();
		void pushPoint(Vector2 waypoint);
	protected:
		unsigned int next;
		std::vector<Vector2> points;
};

//Each PathEdge is a rectangle piece of the path.
struct PathEdge {
	Vector2 vertices[2];
	std::vector<float> costs;
};
Vector2 midpoint(PathEdge edge);

class PathRouter {
	public:
		PathRouter ();
		PathRouter (ResourceManager::GameLevelWrapper newLevel);
		void update (unsigned int edgeIndex, unsigned int costIndex, float newCost);
		//addCost returns cost index of first cost added. Each next cost added is a successive index.
		unsigned int addCost (unsigned int costsAdded);
		unsigned int getEdgeIndex (Vector2 pos);
		WayPath generatePath (Vector2 startPos, Vector2 endPos,std::vector<float> costWeights);
	protected:
		void connectRoutes ();
		bool pathOverlap(Vector2 startOne, Vector2 endOne, Vector2 startTwo, Vector2 endTwo);
		bool pathConnected(Vector2 startOne, Vector2 endOne, Vector2 startTwo, Vector2 endTwo);
		void generatePathPoints(unsigned int startIndex, unsigned int endIndex, float* shortestDist,
				std::vector<unsigned int>* shortestPath, std::vector<float> costWeights,
				float dist=0, std::vector<unsigned int> midPath = std::vector<unsigned int>());
		std::vector<PathEdge> edges;
		std::vector<std::vector<unsigned int>> edgeAdjacency;
};

#endif
