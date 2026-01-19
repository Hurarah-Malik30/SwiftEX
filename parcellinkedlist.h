#ifndef PARCELLINKEDLIST_H
#define PARCELLINKEDLIST_H

#include "parcel.h"

struct ParcelNode {
    Parcel* data;
    ParcelNode* next;
    ParcelNode(Parcel* val);
};

class ParcelLinkedList {
private:
    ParcelNode* head;
    ParcelNode* tail;
public:
    ParcelLinkedList();
    void pushBack(Parcel* val);
    void updateLifecycle(long long currentTime);
    void showTransitStatus(long long currentTime);
};

#endif