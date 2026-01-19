#include "trackinghistory.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>

using namespace std;

// --- THEME COLORS ---
#define RESET     "\033[0m"
#define CYAN      "\033[1;36m"
#define WHITE     "\033[1;37m"
#define GOLD      "\033[1;33m"
#define GRAY      "\033[90m"
#define BG_NAVY   "\033[48;5;18m"
#define BOLD      "\033[1m"

string getCurrentTimestamp() {
    time_t now = time(nullptr);
    tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    stringstream ss;
    ss << setfill('0') << setw(2) << localTime.tm_hour << ":"
        << setw(2) << localTime.tm_min << ":"
        << setw(2) << localTime.tm_sec;

    return ss.str();
}

HistoryEvent::HistoryEvent(string d, string t, string l)
    : description(d), time(t), location(l), next(nullptr) {
}

TrackingHistory::TrackingHistory() : head(nullptr), tail(nullptr) {}

TrackingHistory::TrackingHistory(const TrackingHistory& other) : head(nullptr), tail(nullptr) {
    HistoryEvent* curr = other.head;
    while (curr) {
        addEvent(curr->description, curr->location);
        curr = curr->next;
    }
}

void TrackingHistory::addEvent(string desc, string loc) {
    HistoryEvent* newEvent = new HistoryEvent(desc, getCurrentTimestamp(), loc);
    if (!head) {
        head = tail = newEvent;
    }
    else {
        tail->next = newEvent;
        tail = newEvent;
    }
}

// GUI: Renders a vertical GPS-style timeline within the Navy Theme
void TrackingHistory::printTimeline() {
    string bg = BG_NAVY;

    cout << "\n" << bg << CYAN << "+==========================================================+" << RESET << endl;
    cout << bg << CYAN << "| " << WHITE << BOLD << "              PARCEL JOURNEY TIMELINE                   " << CYAN << "|" << RESET << endl;
    cout << bg << CYAN << "+----------------------------------------------------------+" << RESET << endl;

    HistoryEvent* curr = head;
    if (!curr) {
        cout << bg << "  " << GRAY << "      (No history available for this parcel)            " << RESET << endl;
    }

    while (curr) {
        // Node symbol: (O) for latest, (o) for previous
        string node = (curr->next == nullptr) ? " (O) " : " (o) ";
        string nodeColor = (curr->next == nullptr) ? GOLD : CYAN;

        // Time and Event description
        cout << bg << "  " << GOLD << curr->time << RESET << bg << CYAN << " | " << nodeColor << node << WHITE << left << setw(35) << curr->description << CYAN << "|" << RESET << endl;

        // Location details
        cout << bg << "           " << CYAN << "| " << GRAY << "     L> " << left << setw(33) << curr->location << CYAN << "|" << RESET << endl;

        // Drawing the connector line if there is another event below
        if (curr->next) {
            cout << bg << "           " << CYAN << "| " << GRAY << "      | " << string(34, ' ') << CYAN << "|" << RESET << endl;
        }

        curr = curr->next;
    }

    cout << bg << CYAN << "+==========================================================+" << RESET << endl;
}