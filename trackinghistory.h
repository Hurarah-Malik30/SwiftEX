#ifndef TRACKINGHISTORY_H
#define TRACKINGHISTORY_H

#include <string>

struct HistoryEvent {
    std::string description;
    std::string time;
    std::string location;
    HistoryEvent* next;
    HistoryEvent(std::string d, std::string t, std::string l);
};

class TrackingHistory {
private:
    HistoryEvent* head;
    HistoryEvent* tail;
public:
    TrackingHistory();
    TrackingHistory(const TrackingHistory& other);
    void addEvent(std::string desc, std::string loc);
    void printTimeline();
};

#endif