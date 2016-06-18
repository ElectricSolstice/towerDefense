#include"../towerEntities.h"
#include<allegro5/allegro.h>
#include<allegro5/allegro_image.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_tiled.h>

#include<iostream>
#include<cstdlib>
using namespace std;

class TestPathRouterOne : public PathRouter {
	public:
		TestPathRouterOne (ResourceManager::GameLevelWrapper level) : PathRouter(level) {
			testEdgeFormation();
			testEdgeConnections();
		}
		void testEdgeFormation () {
			assert(edges.size() == 10);
			assert(edges[0].vertices[0].x==160);
			assert(edges[0].vertices[1].x==192);
			assert(edges[0].vertices[0].y==0);
			assert(edges[0].vertices[1].y==32);
		}
		void testEdgeConnections () {
			assert(edgeAdjacency.size() == 10);
		}
};

class TestPathRouterTwo: public PathRouter {
	public:
		TestPathRouterTwo (ResourceManager::GameLevelWrapper level) : PathRouter(level) {
			testEdgeFormation();
			testEdgeConnections();
			testWayPathGeneration();
		}
		void testEdgeFormation () {
			assert(edges.size() == 34);
			assert(edges[0].vertices[0].x==64);
			assert(edges[0].vertices[0].y==0);
			assert(edges[0].vertices[1].x==96);
			assert(edges[0].vertices[1].y==32);
			assert(edges[1].vertices[0].x==64);
			assert(edges[1].vertices[0].y==32);
			assert(edges[1].vertices[1].x==96);
			assert(edges[1].vertices[1].y==64);
			assert(edges[2].vertices[0].x==96);
			assert(edges[2].vertices[0].y==32);
			assert(edges[2].vertices[1].x==128);
			assert(edges[2].vertices[1].y==64);
		}
		void testEdgeConnections () {
			assert(edgeAdjacency.size() == 34);
			assert(edgeAdjacency[0][0] == 1);
			assert(edgeAdjacency[1][0] == 0);
			assert(edgeAdjacency[1][1] == 2);
			assert(edgeAdjacency[2][0] == 1);
			assert(edgeAdjacency[2][1] == 3);
			assert(edgeAdjacency[3][0] == 2);
			assert(edgeAdjacency[3][1] == 4);
			assert(edgeAdjacency[4][0] == 3);
			assert(edgeAdjacency[4][1] == 7);
			assert(edgeAdjacency[7][0] == 4);
			assert(edgeAdjacency[7][1] == 6);
			assert(edgeAdjacency[7][2] == 8);
			assert(edgeAdjacency[7][3] == 11);
			assert(edgeAdjacency[33][0] == 32);
		}
		void printPointError (unsigned int index,WayPath testPath) {
			cout<<"Error on waypoint to edge "<<index<<endl;
			cout<<"Edge Vertexes: "<<edges[index].vertices[0].x<<",";
			cout<<edges[index].vertices[0].y<<endl;
			cout<<"               "<<edges[index].vertices[1].x<<",";
			cout<<edges[index].vertices[1].y<<endl;
			cout<<"Correct Coordinates: ";
			cout<<"("<<midpoint(edges[index]).x<<","<<midpoint(edges[index]).y<<")"<<endl;
			cout<<"Actual Coordinates: ";
			cout<<"("<<testPath.getCurrent().x<<","<<testPath.getCurrent().y<<")"<<endl;
			abort();
		}
		void testPointEqEdge (unsigned int edgeIndex,WayPath testPath) {
			if (testPath.getCurrent().y != midpoint(edges[edgeIndex]).y || \
					testPath.getCurrent().x != midpoint(edges[edgeIndex]).x) {
				printPointError(edgeIndex,testPath);
			}
		}
		void testWayPathGeneration () {
			std::vector<float> costWeights;
			costWeights.push_back(1.0f);
			Vector2 pointOne;
			Vector2 pointTwo = midpoint(edges[33]);
			//testing starting at (0,0) and going to the bottom exit on the map
			pointOne.x = 0.0f;
			pointOne.y = 0.0f;
			WayPath testPath = generatePath(pointOne,pointTwo,costWeights);

			testPointEqEdge(0,testPath);
			testPath.getNext();
			testPointEqEdge(1,testPath);
			testPath.getNext();
			testPointEqEdge(2,testPath);
			testPath.getNext();
			testPointEqEdge(3,testPath);
			testPath.getNext();
			testPointEqEdge(4,testPath);
			testPath.getNext();
			testPointEqEdge(7,testPath);
			testPath.getNext();
			testPointEqEdge(8,testPath);
			testPath.getNext();
			testPointEqEdge(9,testPath);
			testPath.getNext();
			testPointEqEdge(12,testPath);
			testPath.getNext();
			testPointEqEdge(13,testPath);
			testPath.getNext();
			testPointEqEdge(14,testPath);
			testPath.getNext();
			testPointEqEdge(18,testPath);
			testPath.getNext();
			testPointEqEdge(21,testPath);
			testPath.getNext();
			testPointEqEdge(25,testPath);
			testPath.getNext();
			testPointEqEdge(32,testPath);
			testPath.getNext();
			testPointEqEdge(33,testPath);
			assert(!testPath.isNext());
		}
};

int main () {
	if(!al_init()){
		return -1;
	}
	ALLEGRO_DISPLAY *display = al_create_display(800,600);
	if(!display){
		return -1;
	}
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	al_clear_to_color(al_map_rgb(0,0,0));

	al_flip_display();

	ResourceManager::GameLevelWrapper test = ResourceManager::loadGameLevel("pathingTest1.tmx");
	TestPathRouterOne testOne (test);
	test = ResourceManager::loadGameLevel("pathingTest2.tmx");
	TestPathRouterTwo testTwo (test);

	al_destroy_display(display);
	return 0;
}
