#ifndef MAPGRAPH_H
#define MAPGRAPH_H

#include <string>
#include "datastructures.h"

struct Edge {
    int dest;
    int weight;
    bool blocked;
    Edge(int d = 0, int w = 0);
};

class EdgeArrayList {
private:
    Edge* data;
    int capacity;
    int count;
    void resize(int newCapacity);
public:
    EdgeArrayList();
    EdgeArrayList(const EdgeArrayList& other);
    EdgeArrayList& operator=(const EdgeArrayList& other);
    ~EdgeArrayList();
    void add(Edge val);
    int size() const;
    Edge& getRef(int i);
};

struct CityNode {
    std::string name;
    std::string zone;
    EdgeArrayList edges;
    CityNode(std::string n = "", std::string z = "");
};

class MapGraph {
public:
    CityNode* cities;
    int cityCount;
    int cityCapacity;

    bool* visited;
    void solveDFS(int u, int d, IntArrayList currentPath, int currentDist);

    // Store found paths for user selection (Max 5 paths)
    IntArrayList availablePaths[5];
    int availablePathDistances[5];
    int pathCount;

    MapGraph();
    ~MapGraph();

    int addCity(std::string name, std::string zone);
    void addRoad(int u, int v, int dist);
    int getCityIndex(std::string name);
    std::string getZone(std::string name);
    void blockRandomRoad();
    void displayNetwork();
    void findAllPaths(int start, int end);
    int getMinRouteIndex();
};

#endif
