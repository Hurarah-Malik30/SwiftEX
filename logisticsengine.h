#ifndef LOGISTICSENGINE_H
#define LOGISTICSENGINE_H

#include <string>
#include "datastructures.h"
#include "parcellinkedlist.h"
#include "mapgraph.h"

class LogisticsEngine {
private:
    ParcelHashTable database;
    ParcelHeap sortingQueue;
    ParcelLinkedList shippingList;
    StringQueue riderQueue;
    MapGraph map;
    ActionStack undoStack;

    void setupMap();
    void setupRiders();
    void loadFromFile();

public:
    LogisticsEngine();

    void requestPickup(std::string id, std::string dest, double w, int p);
    void processNext();
    void showMap();
    void undoLast();
    void updateRealTime();
    void liveMonitor();
    void viewParcel(std::string id);
    void listAll();
    void saveToFile();
    void cancelParcel(std::string id);
};

#endif