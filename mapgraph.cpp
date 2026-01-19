#include "mapgraph.h"
#include <iostream>
#include <iomanip>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// UI Color Palette for the Terminal GUI
#define RESET   "\033[0m"
#define CYAN    "\033[1;36m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define GOLD    "\033[1;33m"
#define GRAY    "\033[90m"

// =====================================================
// Edge Implementation
// =====================================================
Edge::Edge(int d, int w) : dest(d), weight(w), blocked(false) {}

// =====================================================
// EdgeArrayList Implementation
// =====================================================
void EdgeArrayList::resize(int newCapacity) {
    Edge* newData = new Edge[newCapacity];
    for (int i = 0; i < count; i++)
        newData[i] = data[i];
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

EdgeArrayList::EdgeArrayList() : capacity(5), count(0) {
    data = new Edge[capacity];
}

EdgeArrayList::EdgeArrayList(const EdgeArrayList& other) {
    capacity = other.capacity;
    count = other.count;
    data = new Edge[capacity];
    for (int i = 0; i < count; i++)
        data[i] = other.data[i];
}

EdgeArrayList& EdgeArrayList::operator=(const EdgeArrayList& other) {
    if (this != &other) {
        delete[] data;
        capacity = other.capacity;
        count = other.count;
        data = new Edge[capacity];
        for (int i = 0; i < count; i++)
            data[i] = other.data[i];
    }
    return *this;
}

EdgeArrayList::~EdgeArrayList() {
    delete[] data;
}

void EdgeArrayList::add(Edge val) {
    if (count >= capacity)
        resize(capacity * 2);
    data[count++] = val;
}

int EdgeArrayList::size() const { return count; }
Edge& EdgeArrayList::getRef(int i) { return data[i]; }

// =====================================================
// MapGraph Implementation
// =====================================================
CityNode::CityNode(string n, string z) : name(n), zone(z) {}

MapGraph::MapGraph() : cityCount(0), cityCapacity(15), visited(nullptr) {
    cities = new CityNode[cityCapacity];
}

MapGraph::~MapGraph() {
    delete[] cities;
    if (visited) delete[] visited;
}

int MapGraph::addCity(string name, string zone) {
    if (cityCount >= cityCapacity) return -1;
    cities[cityCount] = CityNode(name, zone);
    return cityCount++;
}

void MapGraph::addRoad(int u, int v, int dist) {
    if (u < cityCount && v < cityCount) {
        cities[u].edges.add(Edge(v, dist));
        cities[v].edges.add(Edge(u, dist));
    }
}

int MapGraph::getCityIndex(string name) {
    for (int i = 0; i < cityCount; i++)
        if (cities[i].name == name) return i;
    return -1;
}

string MapGraph::getZone(string name) {
    int idx = getCityIndex(name);
    return (idx != -1) ? cities[idx].zone : "Unknown";
}

void MapGraph::blockRandomRoad() {
    if (cityCount < 2) return;
    int u = rand() % cityCount;
    if (cities[u].edges.size() > 0) {
        int eIdx = rand() % cities[u].edges.size();
        cities[u].edges.getRef(eIdx).blocked = true;

        cout << RED << "\n [!] LIVE TRAFFIC ALERT: Road near " << cities[u].name
             << " is now BLOCKED due to weather/construction!" << RESET << endl;
    }
}

// GUI-Style Network Display using Universal ASCII Symbols
void MapGraph::displayNetwork() {
    cout << "\n" << CYAN << "+==========================================================+" << RESET << endl;
    cout << CYAN << "|" << RESET << "                SWIFT-EX GEOGRAPHIC NETWORK               " << CYAN << "|" << RESET << endl;
    cout << CYAN << "+----------------------------------------------------------+" << RESET << endl;

    for (int i = 0; i < cityCount; i++) {
        // Formatting the city header line
        cout << CYAN << "|" << RESET << " [" << GOLD << cities[i].zone << RESET << "] "
             << left << setw(15) << cities[i].name << " connects to:" << right << setw(20) << CYAN << "|" << RESET << endl;

        EdgeArrayList& edges = cities[i].edges;
        for (int k = 0; k < edges.size(); k++) {
            Edge& e = edges.getRef(k);
            
            // Fixed String Concatenation logic
            string status = e.blocked ? (string(RED) + "[BLOCKED]" + RESET) : (string(GREEN) + "[OPEN]   " + RESET);

            cout << CYAN << "|" << RESET << "    >> " << left << setw(15) << cities[e.dest].name
                 << " | " << right << setw(4) << e.weight << " km | " << status << right << setw(11) << CYAN << "   |" << RESET << endl;
        }
        
        // Horizontal separator between different cities
        if (i < cityCount - 1)
            cout << CYAN << "|----------------------------------------------------------|" << RESET << endl;
    }
    cout << CYAN << "+==========================================================+" << RESET << endl;
}

// =====================================================
// Pathfinding Logic (DFS with Backtracking)
// =====================================================



void MapGraph::findAllPaths(int start, int end) {
    // Correct memory management for visited array
    if (visited) delete[] visited;
    visited = new bool[cityCount];
    for (int i = 0; i < cityCount; i++) visited[i] = false;

    pathCount = 0;
    IntArrayList currentPath;
    solveDFS(start, end, currentPath, 0);
}

void MapGraph::solveDFS(int u, int d, IntArrayList currentPath, int currentDist) {
    visited[u] = true;
    currentPath.add(u);

    if (u == d) {
        // Prevent buffer overflow (assuming availablePaths size is 5)
        if (pathCount < 5) {
            availablePaths[pathCount] = currentPath;
            availablePathDistances[pathCount] = currentDist;
            pathCount++;
        }
    }
    else {
        EdgeArrayList& edges = cities[u].edges;
        for (int i = 0; i < edges.size(); i++) {
            Edge& e = edges.getRef(i);
            // Only traverse if destination is not visited and road is not blocked
            if (!visited[e.dest] && !e.blocked) {
                solveDFS(e.dest, d, currentPath, currentDist + e.weight);
            }
        }
    }
    // Backtracking: mark as unvisited for other potential paths
    visited[u] = false;
}

int MapGraph::getMinRouteIndex() {
    if (pathCount == 0) return -1;
    int minIdx = 0;
    int minDist = availablePathDistances[0];
    for (int i = 1; i < pathCount; i++) {
        if (availablePathDistances[i] < minDist) {
            minDist = availablePathDistances[i];
            minIdx = i;
        }
    }
    return minIdx;
}