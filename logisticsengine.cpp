#include "logisticsengine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using namespace std;

// --- IMPORTANT: UI Helpers ---
// If these are defined in main.cpp, we declare them here as 'extern' 
// so this file knows they exist without redefining them.
extern void clearScreen();

// UI Color Palette
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[1;36m"
#define GOLD    "\033[1;33m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define GRAY    "\033[90m"
#define BG_BLUE "\033[44m"

LogisticsEngine::LogisticsEngine() {
    srand(static_cast<unsigned int>(time(0)));
    setupMap();
    setupRiders();
    loadFromFile();
}

void LogisticsEngine::setupRiders() {
    riderQueue.enqueue("InamUllah (Light Load)");
    riderQueue.enqueue("Haris Waheed (Heavy Load)");
    riderQueue.enqueue("Ahmad Gulzar (Priority)");
    riderQueue.enqueue("Hurarah (General)");
}

void LogisticsEngine::setupMap() {
    int ccw = map.addCity("Chichawatni", "Zone A");
    int isb = map.addCity("Islamabad", "Zone B");
    int khi = map.addCity("Karachi", "Zone C");
    int psw = map.addCity("Peshawar", "Zone B");
    int mul = map.addCity("Multan", "Zone A");
    int fsd = map.addCity("Faisalabad", "Zone A");
    int que = map.addCity("Quetta", "Zone D");
    int lhr = map.addCity("Lahore", "Zone A");
    int rwp = map.addCity("Rawalpindi", "Zone B");
    int sak = map.addCity("Sakhar", "Zone C");

    map.addRoad(ccw, isb, 375);
    map.addRoad(ccw, fsd, 180);
    map.addRoad(ccw, mul, 345);
    map.addRoad(ccw, lhr, 105);
    map.addRoad(isb, psw, 155);
    map.addRoad(isb, rwp, 20);
    map.addRoad(fsd, lhr, 90);
    map.addRoad(fsd, mul, 240);
    map.addRoad(mul, sak, 490);
    map.addRoad(sak, khi, 470);
    map.addRoad(sak, que, 390);
    map.addRoad(que, khi, 690);
}

void LogisticsEngine::requestPickup(string id, string dest, double w, int p) {
    if (map.getCityIndex(dest) == -1) {
        cout << RED << " [!] Error: Destination city not found in system.\n" << RESET;
        return;
    }

    if (database.search(id)) {
        cout << RED << " [!] Error: Tracking ID " << id << " already exists.\n" << RESET;
        return;
    }

    string zone = map.getZone(dest);
    Parcel* newP = new Parcel(id, dest, w, p, zone);
    database.insert(id, newP);

    newP->updateStatus(STATUS_WAREHOUSE, "Arrived at Warehouse", "Central Hub");
    sortingQueue.insert(newP);
    undoStack.push("ADD", id);

    cout << "\n" << CYAN << " ┌─── SUCCESS: PICKUP LOGGED ───────────────┐" << RESET << endl;
    cout << "   ID: " << BOLD << id << RESET << " | Zone: " << CYAN << zone << RESET << endl;
    cout << "   Destination: " << BOLD << dest << RESET << endl;
    cout << "   Category: " << newP->weightCategory << endl;
    cout << "   Status: " << GREEN << "Sorting Queue" << RESET << endl;
    cout << CYAN << " └──────────────────────────────────────────┘" << RESET << endl;
}

void LogisticsEngine::processNext() {
    if (sortingQueue.isEmpty()) {
        cout << GOLD << " [!] Warehouse Sorting Queue is currently empty.\n" << RESET;
        return;
    }

    if (riderQueue.isEmpty()) {
        cout << RED << " [!] CRITICAL: No Riders available for dispatch!\n" << RESET;
        return;
    }

    Parcel* p = sortingQueue.extractMax();
    string rider = riderQueue.dequeue();
    p->assignedRider = rider;

    int start = map.getCityIndex("Lahore");
    int end = map.getCityIndex(p->destination);

    cout << "\n" << BOLD << " [SYSTEM] Calculating routes for " << p->id << " to " << p->destination << "..." << RESET << endl;
    map.findAllPaths(start, end);

    // FIX: Check pathCount to prevent buffer overruns in availablePaths
    if (map.pathCount <= 0) {
        cout << RED << " [!] ALERT: No valid paths. Returning to Sender.\n" << RESET;
        p->updateStatus(STATUS_RETURNED, "No Route Available", "Warehouse");
        riderQueue.enqueue(rider);
        return;
    }

    int minIdx = map.getMinRouteIndex();
    cout << GRAY << " ──────────────────────────────────────────────────────────" << RESET << endl;
    for (int i = 0; i < map.pathCount; i++) {
        // Safety check for array bounds
        if (i >= 100) break; // Assuming 100 is max capacity

        cout << "  [" << i << "] Distance: " << map.availablePathDistances[i] << " km ";
        if (i == minIdx) cout << GREEN << "(RECOMMENDED)" << RESET;
        cout << "\n   Path: ";
        IntArrayList& path = map.availablePaths[i];
        for (int j = 0; j < path.size(); j++) {
            cout << map.cities[path.get(j)].name << (j < path.size() - 1 ? " -> " : "");
        }
        cout << "\n";
    }
    cout << GRAY << " ──────────────────────────────────────────────────────────" << RESET << endl;

    int choice;
    cout << " Select Route ID to Dispatch " << CYAN << "» " << RESET;
    cin >> choice;

    if (choice < 0 || choice >= map.pathCount) choice = minIdx;

    // Simulate Dynamic Events (Road Blocks)
    if (rand() % 10 < 2) {
        cout << RED << "\n [!] LIVE UPDATE: Road Blockage detected on selected route!" << RESET << endl;
        map.blockRandomRoad();
        cout << " [!] Re-calculating live GPS route..." << endl;
        map.findAllPaths(start, end);
        if (map.pathCount > 0) {
            choice = map.getMinRouteIndex();
            cout << GREEN << " [✓] Rerouted to new shortest path." << RESET << endl;
        }
    }

    p->updateStatus(STATUS_LOADING, "Loading onto Truck", "Bay 4");
    long long travelSecs = 15 + (rand() % 30);
    p->dispatchTime = time(0);
    p->arrivalTime = time(0) + travelSecs;

    shippingList.pushBack(p);
    undoStack.push("DISPATCH", p->id);

    cout << "\n" << GREEN << " [✓] DISPATCH SUCCESSFUL" << RESET << endl;
    cout << "   Rider: " << rider << " | ETA: " << travelSecs << "s\n";

    riderQueue.enqueue(rider);
}

void LogisticsEngine::showMap() {
    cout << CYAN << "\n [ GEOGRAPHIC LOGISTICS NETWORK ]\n" << RESET;
    map.displayNetwork();
}

void LogisticsEngine::undoLast() {
    UndoAction act;
    if (undoStack.pop(act)) {
        Parcel* p = database.search(act.parcelId);
        if (p) {
            if (act.type == "ADD") {
                p->updateStatus(STATUS_CANCELLED, "Undo: Creation Reverted", "N/A");
                cout << GOLD << " [Undo] Parcel " << p->id << " creation cancelled.\n" << RESET;
            }
            else if (act.type == "DISPATCH") {
                p->updateStatus(STATUS_WAREHOUSE, "Undo: Dispatch Reverted", "Warehouse");
                p->arrivalTime = 0;
                sortingQueue.insert(p);
                cout << GOLD << " [Undo] Parcel " << p->id << " pulled back to warehouse.\n" << RESET;
            }
        }
    }
    else {
        cout << GRAY << " [!] Nothing to undo.\n" << RESET;
    }
}

void LogisticsEngine::updateRealTime() {
    shippingList.updateLifecycle(static_cast<long long>(time(0)));
}

void LogisticsEngine::liveMonitor() {
    char cmd = 'r';
    while (cmd == 'r' || cmd == 'R') {
        clearScreen();
        cout << BG_BLUE << "   LIVE TRANSIT MONITOR   " << RESET << "\n\n";
        updateRealTime();
        shippingList.showTransitStatus(static_cast<long long>(time(0)));
        cout << GRAY << "\n ──────────────────────────────────────────" << RESET << endl;
        cout << " [r] Refresh Data   [x] Main Menu » ";
        cin >> cmd;
    }
}

void LogisticsEngine::viewParcel(string id) {
    Parcel* p = database.search(id);
    if (p) {
        cout << "\n" << BOLD << CYAN << " ════════════ TRACKING: " << p->id << " ════════════" << RESET << endl;
        cout << *p << endl;

        cout << " Progress: [";
        for (int i = 0; i < 6; i++) {
            if (i <= p->status) cout << GREEN << "■" << RESET;
            else cout << GRAY << "□" << RESET;
        }
        cout << "]\n\n";

        p->history->printTimeline();

        if (p->status == STATUS_IN_TRANSIT) {
            long long rem = p->arrivalTime - static_cast<long long>(time(0));
            if (rem > 0) cout << CYAN << "\n >>> LIVE ETA: " << rem << " seconds" << RESET << endl;
        }
    }
    else {
        cout << RED << " [!] Tracking ID not found in database.\n" << RESET;
    }
}

void LogisticsEngine::listAll() {
    cout << CYAN << "\n [ COMPLETE INVENTORY RECORDS ]\n" << RESET;
    database.printAll();
}

void LogisticsEngine::saveToFile() {
    ofstream f("parcels.txt");
    if (f.is_open()) {
        database.saveToFile(f);
        f.close();
        cout << GREEN << " [✓] Data synced to parcels.txt\n" << RESET;
    }
}

void LogisticsEngine::loadFromFile() {
    ifstream f("parcels.txt");
    if (f.is_open()) {
        string line, id, dest, temp, zone;
        while (getline(f, line)) {
            stringstream ss(line);
            getline(ss, id, ',');
            getline(ss, dest, ',');
            getline(ss, temp, ',');
            if (temp.empty()) continue;
            double w = stod(temp);
            getline(ss, temp, ','); int p = stoi(temp);
            getline(ss, temp, ','); int s = stoi(temp);
            getline(ss, zone, ',');

            Parcel* newP = new Parcel(id, dest, w, p, zone);
            newP->status = s;
            database.insert(id, newP);

            if (s == STATUS_WAREHOUSE) sortingQueue.insert(newP);
            if (s >= STATUS_LOADING && s <= STATUS_DELIVERY_ATTEMPT) shippingList.pushBack(newP);
        }
        f.close();
    }
}

void LogisticsEngine::cancelParcel(string id) {
    Parcel* p = database.search(id);
    if (p && p->status <= STATUS_WAREHOUSE) {
        p->updateStatus(STATUS_CANCELLED, "Cancelled by Admin", "Warehouse");
        cout << GREEN << " [✓] Parcel " << id << " cancelled successfully.\n" << RESET;
    }
    else {
        cout << RED << " [!] Cannot cancel: Parcel is already in transit or missing.\n" << RESET;
    }
}