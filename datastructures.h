#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <string>
#include "parcel.h"

// Forward declarations
struct HistoryEvent;
class TrackingHistory;
struct ParcelNode;
class ParcelLinkedList;
class EdgeArrayList;

// StringQueue
class StringQueue {
private:
    struct Node {
        std::string data;
        Node* next;
        Node(std::string d);
    };
    Node* front;
    Node* rear;
public:
    StringQueue();
    void enqueue(std::string val);
    std::string dequeue();
    bool isEmpty();
};

// IntArrayList
class IntArrayList {
private:
    int* data;
    int capacity;
    int count;
    void resize(int newCapacity);
public:
    IntArrayList();
    IntArrayList(const IntArrayList& other);
    IntArrayList& operator=(const IntArrayList& other);
    ~IntArrayList();
    void add(int val);
    int get(int index) const;
    int size() const;
    bool isEmpty() const;
};

// ParcelArrayList
class ParcelArrayList {
private:
    Parcel** data;
    int capacity;
    int count;
    void resize(int newCapacity);
public:
    ParcelArrayList();
    ~ParcelArrayList();
    void add(Parcel* val);
    void removeLast();
    Parcel* get(int index) const;
    void set(int index, Parcel* val);
    int size() const;
    bool isEmpty() const;
    void swap(int i, int j);
};

// ParcelHeap
class ParcelHeap {
private:
    ParcelArrayList heap;
    void heapifyUp(int index);
    void heapifyDown(int index);
public:
    void insert(Parcel* val);
    Parcel* extractMax();
    bool isEmpty();
};

// ParcelHashTable
struct HashEntry {
    std::string key;
    Parcel* value;
    bool occupied;
    HashEntry();
};

class ParcelHashTable {
private:
    HashEntry* table;
    int capacity;
    int hashFunction(const std::string& key);
public:
    ParcelHashTable(int cap = 1007);
    ~ParcelHashTable();
    void insert(std::string key, Parcel* value);
    Parcel* search(std::string key);
    void printAll();
    void saveToFile(std::ofstream& out);
};

// ActionStack
struct UndoAction {
    std::string type;
    std::string parcelId;
};

class ActionStack {
private:
    struct UndoNode {
        UndoAction data;
        UndoNode* next;
        UndoNode(UndoAction d);
    };
    UndoNode* topNode;
public:
    ActionStack();
    void push(std::string type, std::string id);
    bool pop(UndoAction& dest);
};

#endif