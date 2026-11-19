#include <iostream>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET socket_t;
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <strings.h>
  typedef int socket_t;
  #define INVALID_SOCKET (-1)
  #define SOCKET_ERROR (-1)
#endif
using namespace std;
const int NAME_LEN   = 16;
const int MAX_BLOCKS = 10;

inline void copyStr(char* destination, const char* source, int size) {
    strncpy(destination, source, size - 1);
    destination[size - 1] = '\0';
}

template <class T>
void genericSwap(T& first, T& second) {
    T temp = first;
    first  = second;
    second = temp;
}

class ParkingException {
    char message[100];
public:
    ParkingException(const char* msg) { copyStr(message, msg, 100); }
    const char* what() const { return message; }
};
class Vehicle {
protected:
    char number[NAME_LEN];
    int  ownerLevel;
public:
    Vehicle(const char* vehicleNo, int level);
    virtual ~Vehicle();

    virtual int getPriority() const = 0;
    virtual int getFineRate() const = 0;
    virtual const char* getType() const = 0;

    const char* getNumber() const { return number; }
};

class TwoWheeler : public Vehicle {
public:
    TwoWheeler(const char* vehicleNo, int level);
    int getPriority() const override;
    int getFineRate() const override;
    const char* getType() const override;
};

class FourWheeler : public Vehicle {
public:
    FourWheeler(const char* vehicleNo, int level);
    int getPriority() const override;
    int getFineRate() const override;
    const char* getType() const override;
};

class EmergencyVehicle : public Vehicle {
public:
    EmergencyVehicle(const char* vehicleNo);
    int getPriority() const override;
    int getFineRate() const override;
    const char* getType() const override;
};

struct NoShowNode {
    char vehicleNo[NAME_LEN];
    int  count;
    NoShowNode* left;
    NoShowNode* right;
};

class NoShowTree {
    NoShowNode* root;

    NoShowNode* insert(NoShowNode* node, const char* vehicleNo);
    void inorder(NoShowNode* node) const;
    void destroy(NoShowNode* node);

    NoShowTree(const NoShowTree&);
    NoShowTree& operator=(const NoShowTree&);
public:
    NoShowTree();
    ~NoShowTree();
    void recordNoShow(const char* vehicleNo);
    int  getNoShowCount(const char* vehicleNo) const;
    bool isRepeatOffender(const char* vehicleNo) const;
    bool getEntryAt(int index, char* vehicleNoOut, int outSize, int& countOut) const;
    void display() const;
private:
    bool getEntryAtInorder(NoShowNode* node, int& current, int target, char* out, int outSize, int& countOut) const;
};

class UndoStack {
    int data[100];
    int top;
public:
    UndoStack();
    bool push(int reservationId);
    bool pop(int& reservationId);
    bool peek(int& reservationId) const;
    bool isEmpty() const;
    bool isFull() const;
};

Vehicle::Vehicle(const char* vehicleNo, int level) {
    copyStr(number, vehicleNo, NAME_LEN);
    if (level == 1) {
        ownerLevel = 1;
    } else {
        ownerLevel = 2;
    }
}
Vehicle::~Vehicle() {}

TwoWheeler::TwoWheeler(const char* vehicleNo, int level) : Vehicle(vehicleNo, level) {}
int TwoWheeler::getPriority() const { return ownerLevel; }
int TwoWheeler::getFineRate() const { return 20; }
const char* TwoWheeler::getType() const { return "TwoWheeler"; }

FourWheeler::FourWheeler(const char* vehicleNo, int level) : Vehicle(vehicleNo, level) {}
int FourWheeler::getPriority() const { return ownerLevel; }
int FourWheeler::getFineRate() const { return 50; }
const char* FourWheeler::getType() const { return "FourWheeler"; }

EmergencyVehicle::EmergencyVehicle(const char* vehicleNo) : Vehicle(vehicleNo, 1) {}
int EmergencyVehicle::getPriority() const { return 0; }
int EmergencyVehicle::getFineRate() const { return 0; }
const char* EmergencyVehicle::getType() const { return "Emergency"; }

NoShowTree::NoShowTree() : root(0) {}
NoShowTree::~NoShowTree() { destroy(root); }

void NoShowTree::destroy(NoShowNode* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

NoShowNode* NoShowTree::insert(NoShowNode* node, const char* vehicleNo) {
    if (!node) {
        node = new NoShowNode;
        copyStr(node->vehicleNo, vehicleNo, NAME_LEN);
        node->count = 1;
        node->left = node->right = 0;
        return node;
    }
    int comparison = strcmp(vehicleNo, node->vehicleNo);
    if (comparison == 0)      node->count++;
    else if (comparison < 0)  node->left  = insert(node->left, vehicleNo);
    else                      node->right = insert(node->right, vehicleNo);
    return node;
}

void NoShowTree::recordNoShow(const char* vehicleNo) { root = insert(root, vehicleNo); }

int NoShowTree::getNoShowCount(const char* vehicleNo) const {
    NoShowNode* current = root;
    while (current) {
        int comparison = strcmp(vehicleNo, current->vehicleNo);
        if (comparison == 0) return current->count;
        if (comparison < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return 0;
}

bool NoShowTree::isRepeatOffender(const char* vehicleNo) const {
    return getNoShowCount(vehicleNo) >= 3;
}

bool NoShowTree::getEntryAtInorder(NoShowNode* node, int& current, int target, char* out, int outSize, int& countOut) const {
    if (!node) return false;
    if (getEntryAtInorder(node->left, current, target, out, outSize, countOut)) return true;
    if (current == target) {
        copyStr(out, node->vehicleNo, outSize);
        countOut = node->count;
        return true;
    }
    current++;
    return getEntryAtInorder(node->right, current, target, out, outSize, countOut);
}
bool NoShowTree::getEntryAt(int index, char* vehicleNoOut, int outSize, int& countOut) const {
    if (index < 0 || !vehicleNoOut || outSize <= 0) return false;
    int current = 0;
    return getEntryAtInorder(root, current, index, vehicleNoOut, outSize, countOut);
}

void NoShowTree::inorder(NoShowNode* node) const {
    if (!node) return;
    inorder(node->left);
    cout << "  " << node->vehicleNo << "  no-shows: " << node->count << endl;
    inorder(node->right);
}

void NoShowTree::display() const {
    cout << "No-show register (sorted by vehicle number):" << endl;
    if (!root) cout << "  (empty)" << endl;
    inorder(root);
}

UndoStack::UndoStack() : top(-1) {}
bool UndoStack::isEmpty() const { return top == -1; }
bool UndoStack::isFull()  const { return top == 99; }
bool UndoStack::push(int reservationId) {
    if (isFull()) return false;
    top++;
    data[top] = reservationId;
    return true;
}
bool UndoStack::pop(int& reservationId) {
    if (isEmpty()) return false;
    reservationId = data[top];
    top--;
    return true;
}
bool UndoStack::peek(int& reservationId) const {
    if (isEmpty()) return false;
    reservationId = data[top];
    return true;
}
