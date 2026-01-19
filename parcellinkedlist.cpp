#include "parcellinkedlist.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

// UI Color Palette
#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define CYAN      "\033[1;36m"
#define GREEN     "\033[1;32m"
#define GOLD      "\033[1;33m"
#define WHITE     "\033[1;37m"
#define GRAY      "\033[90m"
#define BG_NAVY   "\033[48;5;18m"

ParcelNode::ParcelNode(Parcel* val) : data(val), next(nullptr) {}

ParcelLinkedList::ParcelLinkedList() : head(nullptr), tail(nullptr) {}

void ParcelLinkedList::pushBack(Parcel* val) {
    ParcelNode* newNode = new ParcelNode(val);
    if (!tail) {
        head = tail = newNode;
    }
    else {
        tail->next = newNode;
        tail = newNode;
    }
}

// Logic: Handles the "Auto-moving" of parcels through the linked list


void ParcelLinkedList::updateLifecycle(long long currentTime) {
    ParcelNode* curr = head;
    while (curr) {
        Parcel* p = curr->data;
        if (p->status == STATUS_LOADING) {
            if (currentTime >= p->lastUpdateTime + 5) {
                p->updateStatus(STATUS_IN_TRANSIT, "Vehicle Departed", "On Road");
            }
        }
        else if (p->status == STATUS_IN_TRANSIT) {
            // 0.1% chance of parcel going missing for realism
            if (rand() % 1000 == 0) {
                p->updateStatus(STATUS_MISSING, "Signal Lost - Investigation Started", "Unknown");
            }
            else if (currentTime >= p->arrivalTime) {
                p->updateStatus(STATUS_DELIVERY_ATTEMPT, "Arrived at Destination Hub", p->destination);
            }
        }
        else if (p->status == STATUS_DELIVERY_ATTEMPT) {
            // 80% success rate for first delivery attempt
            if (rand() % 10 < 8) {
                p->updateStatus(STATUS_DELIVERED, "Handed to Recipient", "Doorstep");
            }
            else {
                p->deliveryAttempts++;
                if (p->deliveryAttempts >= 3) {
                    p->updateStatus(STATUS_RETURNED, "Max Attempts Reached - RTS", "Local Hub");
                }
                else {
                    p->updateStatus(STATUS_IN_TRANSIT, "Recipient Unavailable - Retrying", "Local Hub");
                    p->arrivalTime = currentTime + 5; // Re-schedule transit time
                }
            }
        }
        curr = curr->next;
    }
}

// GUI: Displays the progress bars for all active deliveries
void ParcelLinkedList::showTransitStatus(long long currentTime) {
    ParcelNode* curr = head;
    bool headerPrinted = false;
    string bg = BG_NAVY;

    while (curr) {
        // Only show parcels that are actually moving or being loaded
        if (curr->data->status == STATUS_IN_TRANSIT || curr->data->status == STATUS_LOADING) {
            if (!headerPrinted) {
                cout << bg << CYAN << "+==========================================================+" << RESET << endl;
                cout << bg << CYAN << "| " << WHITE << BOLD << "              LIVE FLEET TRANSIT MONITOR                " << CYAN << "|" << RESET << endl;
                cout << bg << CYAN << "+----------------------------------------------------------+" << RESET << endl;
                headerPrinted = true;
            }

            long long total = curr->data->arrivalTime - curr->data->dispatchTime;
            long long elapsed = currentTime - curr->data->dispatchTime;

            string state = (curr->data->status == STATUS_LOADING) ?
                string(GOLD) + "[LOADING]" + WHITE :
                string(GREEN) + "[MOVING ]" + WHITE;

            if (total <= 0) total = 1;
            double pct = (double)elapsed / total;
            if (pct > 1.0) pct = 1.0;
            if (pct < 0.0) pct = 0.0;

            // Render the Bar
            cout << bg << "  " << state << " " << left << setw(8) << curr->data->id << " » "
                << left << setw(12) << curr->data->destination << " " << CYAN << "[";

            int bars = (int)(pct * 15);
            for (int i = 0; i < 15; i++) {
                if (i < bars) cout << "■";
                else cout << " ";
            }

            cout << "] " << WHITE << setw(3) << (int)(pct * 100) << "% " << CYAN << "|" << RESET << endl;
        }
        curr = curr->next;
    }

    if (headerPrinted) {
        cout << bg << CYAN << "+==========================================================+" << RESET << endl;
    }
    else {
        cout << bg << GRAY << "        (No active transit signals detected)              " << RESET << endl;
    }
}