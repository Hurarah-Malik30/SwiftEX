#include "datastructures.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <climits>
#include <cstdlib>
#include <ctime>

using namespace std;

// =====================================================
// StringQueue Implementation (Linked List based)
// =====================================================
StringQueue::Node::Node(string d) : data(d), next(nullptr) {}

StringQueue::StringQueue() : front(nullptr), rear(nullptr) {}

void StringQueue::enqueue(string val) {
    Node* n = new Node(val);
    if (!rear)
        front = rear = n;
    else {
        rear->next = n;
        rear = n;
    }
}

string StringQueue::dequeue() {
    if (!front) return "";
    Node* temp = front;
    string val = front->data;
    front = front->next;
    if (!front) rear = nullptr;
    delete temp;
    return val;
}

bool StringQueue::isEmpty() {
    return front == nullptr;
}

// =====================================================
// IntArrayList Implementation (Dynamic Array)
// =====================================================
void IntArrayList::resize(int newCapacity) {
    int* newData = new int[newCapacity];
    for (int i = 0; i < count; i++) {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

IntArrayList::IntArrayList() : capacity(50), count(0) {
    data = new int[capacity];
}

IntArrayList::IntArrayList(const IntArrayList& other) {
    capacity = other.capacity;
    count = other.count;
    data = new int[capacity];
    for (int i = 0; i < count; i++)
        data[i] = other.data[i];
}

IntArrayList& IntArrayList::operator=(const IntArrayList& other) {
    if (this != &other) {
        delete[] data;
        capacity = other.capacity;
        count = other.count;
        data = new int[capacity];
        for (int i = 0; i < count; i++)
            data[i] = other.data[i];
    }
    return *this;
}

IntArrayList::~IntArrayList() {
    delete[] data;
}

void IntArrayList::add(int val) {
    if (count >= capacity)
        resize(capacity * 2);
    data[count++] = val;
}

int IntArrayList::get(int index) const {
    if (index < 0 || index >= count) return -1;
    return data[index];
}

int IntArrayList::size() const { return count; }

bool IntArrayList::isEmpty() const { return count == 0; }

// =====================================================
// ParcelArrayList Implementation (Pointers Array)
// =====================================================
void ParcelArrayList::resize(int newCapacity) {
    Parcel** newData = new Parcel * [newCapacity];
    for (int i = 0; i < count; i++)
        newData[i] = data[i];
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

ParcelArrayList::ParcelArrayList() : capacity(20), count(0) {
    data = new Parcel * [capacity];
}

ParcelArrayList::~ParcelArrayList() {
    delete[] data;
}

void ParcelArrayList::add(Parcel* val) {
    if (count >= capacity)
        resize(capacity * 2);
    data[count++] = val;
}

void ParcelArrayList::removeLast() {
    if (count > 0) count--;
}

Parcel* ParcelArrayList::get(int index) const {
    if (index < 0 || index >= count) return nullptr;
    return data[index];
}

void ParcelArrayList::set(int index, Parcel* val) {
    if (index >= 0 && index < count)
        data[index] = val;
}

int ParcelArrayList::size() const { return count; }

bool ParcelArrayList::isEmpty() const { return count == 0; }

void ParcelArrayList::swap(int i, int j) {
    Parcel* temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}

// =====================================================
// ParcelHeap Implementation (Priority Queue)
// =====================================================


void ParcelHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap.get(index)->priorityScore > heap.get(parent)->priorityScore) {
            heap.swap(index, parent);
            index = parent;
        }
        else break;
    }
}

void ParcelHeap::heapifyDown(int index) {
    int left = 2 * index + 1, right = 2 * index + 2, largest = index;

    if (left < heap.size() && heap.get(left)->priorityScore > heap.get(largest)->priorityScore)
        largest = left;
    if (right < heap.size() && heap.get(right)->priorityScore > heap.get(largest)->priorityScore)
        largest = right;

    if (largest != index) {
        heap.swap(index, largest);
        heapifyDown(largest);
    }
}

void ParcelHeap::insert(Parcel* val) {
    heap.add(val);
    heapifyUp(heap.size() - 1);
}

Parcel* ParcelHeap::extractMax() {
    if (heap.isEmpty()) return nullptr;

    Parcel* maxVal = heap.get(0);
    heap.set(0, heap.get(heap.size() - 1));
    heap.removeLast();
    if (!heap.isEmpty())
        heapifyDown(0);

    return maxVal;
}

bool ParcelHeap::isEmpty() { return heap.isEmpty(); }

// =====================================================
// ParcelHashTable Implementation (Quadratic Probing)
// =====================================================
HashEntry::HashEntry() : key(""), value(nullptr), occupied(false) {}

ParcelHashTable::ParcelHashTable(int cap) : capacity(cap) {
    table = new HashEntry[capacity];
}

ParcelHashTable::~ParcelHashTable() {
    delete[] table;
}

int ParcelHashTable::hashFunction(const string& key) {
    unsigned long hash = 5381;
    for (char c : key)
        hash = ((hash << 5) + hash) + c;
    return hash % capacity;
}

void ParcelHashTable::insert(string key, Parcel* value) {
    int index = hashFunction(key);
    for (int i = 0; i < capacity; i++) {
        int probe = (index + i * i) % capacity;
        if (!table[probe].occupied || table[probe].key == key) {
            table[probe].key = key;
            table[probe].value = value;
            table[probe].occupied = true;
            return;
        }
    }
}

Parcel* ParcelHashTable::search(string key) {
    int index = hashFunction(key);
    for (int i = 0; i < capacity; i++) {
        int probe = (index + i * i) % capacity;
        if (!table[probe].occupied) return nullptr;
        if (table[probe].key == key) return table[probe].value;
    }
    return nullptr;
}

void ParcelHashTable::printAll() {
    string cyan = "\033[1;36m";
    string reset = "\033[0m";

    cout << "\n" << cyan << "┌──────────────────────────────────────────────────────────────────┐" << reset << endl;
    cout << cyan << "│" << reset << "                SWIFT-EX CENTRAL PARCEL DATABASE                  " << cyan << "│" << reset << endl;
    cout << cyan << "├────────────┬───────────────┬────────────┬───────────┬────────────┤" << reset << endl;
    cout << cyan << "│" << reset << " ID         " << cyan << "│" << reset << " DESTINATION   " << cyan << "│" << reset << " WT (KG)    " << cyan << "│" << reset << " ZONE      " << cyan << "│" << reset << " STATUS     " << cyan << "│" << reset << endl;
    cout << cyan << "├────────────┼───────────────┼────────────┼───────────┼────────────┤" << reset << endl;

    for (int i = 0; i < capacity; i++) {
        if (table[i].occupied) {
            Parcel* p = table[i].value;
            cout << cyan << "│ " << reset << left << setw(11) << p->id
                << cyan << "│ " << reset << left << setw(14) << p->destination
                << cyan << "│ " << reset << left << setw(11) << p->weight
                << cyan << "│ " << reset << left << setw(10) << p->zone
                << cyan << "│ " << reset << left << setw(11) << p->getStatusString() << cyan << "│" << reset << endl;
        }
    }
    cout << cyan << "└────────────┴───────────────┴────────────┴───────────┴────────────┘" << reset << endl;
}

void ParcelHashTable::saveToFile(ofstream& out) {
    for (int i = 0; i < capacity; i++)
        if (table[i].occupied) {
            Parcel* p = table[i].value;
            out << p->id << "," << p->destination << "," << p->weight << ","
                << p->priority << "," << p->status << "," << p->zone << endl;
        }
}

// =====================================================
// ActionStack Implementation (Undo Functionality)
// =====================================================


ActionStack::UndoNode::UndoNode(UndoAction d) : data(d), next(nullptr) {}

ActionStack::ActionStack() : topNode(nullptr) {}

void ActionStack::push(string type, string id) {
    UndoNode* n = new UndoNode({ type, id });
    n->next = topNode;
    topNode = n;
}

bool ActionStack::pop(UndoAction& dest) {
    if (!topNode) return false;

    UndoNode* temp = topNode;
    dest = temp->data;
    topNode = topNode->next;
    delete temp;
    return true;
}