#include "parcel.h"
#include <ctime>
#include <iomanip>
#include <iostream>

// Color Macros for Status Badges
#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define BG_GREEN  "\033[42;1m\033[38;5;15m" // Green BG, White Text
#define BG_BLUE   "\033[44;1m\033[38;5;15m" // Blue BG, White Text
#define BG_YELLOW "\033[43;1m\033[30m"      // Yellow BG, Black Text
#define BG_RED    "\033[41;1m\033[38;5;15m" // Red BG, White Text
#define BG_GRAY   "\033[100;1m\033[38;5;15m"// Gray BG

Parcel::Parcel() : weight(0), priority(1), status(0), priorityScore(0),
history(new TrackingHistory()), dispatchTime(0),
lastUpdateTime(0), arrivalTime(0), deliveryAttempts(0) {
}

Parcel::Parcel(std::string pid, std::string dest, double w, int p, std::string z)
    : id(pid), destination(dest), weight(w), priority(p), status(0),
    lastUpdateTime(0), arrivalTime(0), zone(z), deliveryAttempts(0), dispatchTime(0) {

    priorityScore = p * 1000 + (int)w;

    if (w < 5.0) weightCategory = "Light";
    else if (w < 20.0) weightCategory = "Medium";
    else weightCategory = "Heavy";

    history = new TrackingHistory();
    history->addEvent("Pickup Request Created", "Customer Loc");
}

void Parcel::updateStatus(int newStatus, std::string desc, std::string loc) {
    status = newStatus;
    history->addEvent(desc, loc);
    lastUpdateTime = time(0);
}

// Color-coded status strings for the "GUI" look
std::string Parcel::getStatusString() const {
    switch (status) {
    case 0: return std::string(BG_GRAY) + " PICKUP QUEUE " + RESET;
    case 1: return std::string(BG_BLUE) + " WAREHOUSE    " + RESET;
    case 2: return std::string(BG_BLUE) + " LOADING      " + RESET;
    case 3: return std::string(BG_YELLOW) + " IN TRANSIT   " + RESET;
    case 4: return std::string(BG_YELLOW) + " OUT FOR DEL. " + RESET;
    case 5: return std::string(BG_GREEN) + " DELIVERED    " + RESET;
    case 6: return std::string(BG_RED) + " RETURNED     " + RESET;
    case 7: return std::string(BG_RED) + " MISSING      " + RESET;
    case 8: return std::string(BG_GRAY) + " CANCELLED    " + RESET;
    default: return "Unknown";
    }
}

// Overloaded operator for the Dashboard Table
std::ostream& operator<<(std::ostream& os, const Parcel& p) {
    // We use Fixed widths to keep the table rows perfectly aligned
    os << " " << std::left << std::setw(10) << p.id
        << std::setw(15) << p.destination
        << std::setw(10) << p.weightCategory
        << std::setw(10) << p.zone
        << " " << p.getStatusString();
    return os;
}