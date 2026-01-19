
#ifndef PARCEL_H
#define PARCEL_H

#include <string>
#include "trackinghistory.h"

const int STATUS_PICKUP_QUEUE = 0;
const int STATUS_WAREHOUSE = 1;
const int STATUS_LOADING = 2;
const int STATUS_IN_TRANSIT = 3;
const int STATUS_DELIVERY_ATTEMPT = 4;
const int STATUS_DELIVERED = 5;
const int STATUS_RETURNED = 6;
const int STATUS_MISSING = 7;
const int STATUS_CANCELLED = 8;

struct Parcel {
    std::string id;
    std::string destination;
    double weight;
    int priority;
    int status;
    int priorityScore;
    std::string assignedRider;
    std::string weightCategory;
    std::string zone;
    int deliveryAttempts;
    long long dispatchTime;
    long long lastUpdateTime;
    long long arrivalTime;
    TrackingHistory* history;

    Parcel();
    Parcel(std::string pid, std::string dest, double w, int p, std::string z);
    void updateStatus(int newStatus, std::string desc, std::string loc);
    std::string getStatusString() const;
};

std::ostream& operator<<(std::ostream& os, const Parcel& p);

#endif