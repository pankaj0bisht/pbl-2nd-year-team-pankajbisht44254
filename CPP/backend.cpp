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

enum SlotState { SLOT_FREE, SLOT_RESERVED, SLOT_OCCUPIED, SLOT_EXPIRED };

class ParkingSlot {
    SlotState state;
public:
    ParkingSlot();
    SlotState getState() const;
    bool isFree() const;
    bool reserve();
    bool markOccupied();
    bool markExpired();
    bool release();
    const char* stateName() const;
};

class ParkingBlock {
    char id[NAME_LEN];
    int capacity;
    ParkingSlot* slots;
public:
    ParkingBlock(const char* blockId, int numSlots);
    ParkingBlock(const ParkingBlock& other);
    ParkingBlock& operator=(const ParkingBlock& other);
    ~ParkingBlock();

    const char* getId() const { return id; }
    int getCapacity() const { return capacity; }
    SlotState getSlotStateAt(int index) const {
        return (index >= 0 && index < capacity)
            ? slots[index].getState() : SLOT_FREE;
    }
    int findFreeSlotIndex() const;
    bool hasFreeSlot() const;
    int countByState(SlotState s) const;
    int usedCount() const;
    bool reserveSlot(int index);
    bool occupySlot(int index);
    bool expireSlot(int index);
    bool releaseSlot(int index);

    friend ostream& operator<<(ostream& out, const ParkingBlock& b);
};

namespace BlockUtils {
    void insertionSortById(ParkingBlock** arr, int n);
    int binarySearchById(ParkingBlock** arr, int n, const char* id);
    void quickSortByOccupancy(ParkingBlock** arr, int low, int high);
}

ParkingSlot::ParkingSlot() : state(SLOT_FREE) {}

SlotState ParkingSlot::getState() const {
    return state;
}

bool ParkingSlot::isFree() const {
    return state == SLOT_FREE;
}

bool ParkingSlot::reserve() {
    if (state != SLOT_FREE) return false;
    state = SLOT_RESERVED;
    return true;
}

bool ParkingSlot::markOccupied() {
    if (state != SLOT_RESERVED) return false;
    state = SLOT_OCCUPIED;
    return true;
}

bool ParkingSlot::markExpired() {
    if (state != SLOT_RESERVED) return false;
    state = SLOT_EXPIRED;
    return true;
}

bool ParkingSlot::release() {
    state = SLOT_FREE;
    return true;
}

const char* ParkingSlot::stateName() const {
    switch (state) {
        case SLOT_FREE: return "FREE";
        case SLOT_RESERVED: return "RESERVED";
        case SLOT_OCCUPIED: return "OCCUPIED";
        default: return "EXPIRED";
    }
}

ParkingBlock::ParkingBlock(const char* blockId, int numSlots) {
    copyStr(id, blockId, NAME_LEN);

    if (numSlots > 0) {
        capacity = numSlots;
    } else {
        capacity = 1;
    }

    slots = new ParkingSlot[capacity];
}

ParkingBlock::ParkingBlock(const ParkingBlock& other) {
    copyStr(id, other.id, NAME_LEN);
    capacity = other.capacity;
    slots = new ParkingSlot[capacity];

    for (int i = 0; i < capacity; i++)
        slots[i] = other.slots[i];
}

ParkingBlock& ParkingBlock::operator=(const ParkingBlock& other) {
    if (this == &other) return *this;

    delete[] slots;
    copyStr(id, other.id, NAME_LEN);
    capacity = other.capacity;
    slots = new ParkingSlot[capacity];

    for (int i = 0; i < capacity; i++)
        slots[i] = other.slots[i];

    return *this;
}

ParkingBlock::~ParkingBlock() {
    delete[] slots;
}

int ParkingBlock::findFreeSlotIndex() const {
    for (int i = 0; i < capacity; i++)
        if (slots[i].isFree()) return i;

    return -1;
}

bool ParkingBlock::hasFreeSlot() const {
    return findFreeSlotIndex() != -1;
}

int ParkingBlock::countByState(SlotState s) const {
    int total = 0;

    for (int i = 0; i < capacity; i++)
        if (slots[i].getState() == s) total++;

    return total;
}

int ParkingBlock::usedCount() const {
    return capacity - countByState(SLOT_FREE);
}

bool ParkingBlock::reserveSlot(int index) {
    return (index >= 0 && index < capacity)
        && slots[index].reserve();
}

bool ParkingBlock::occupySlot(int index) {
    return (index >= 0 && index < capacity)
        && slots[index].markOccupied();
}

bool ParkingBlock::expireSlot(int index) {
    return (index >= 0 && index < capacity)
        && slots[index].markExpired();
}

bool ParkingBlock::releaseSlot(int index) {
    return (index >= 0 && index < capacity)
        && slots[index].release();
}

ostream& operator<<(ostream& out, const ParkingBlock& b) {
    out << "Block " << b.id << " ["
        << b.usedCount() << "/" << b.capacity << " used]  ";

    for (int i = 0; i < b.capacity; i++)
        out << "(" << i << ":" << b.slots[i].stateName() << ") ";

    return out;
}

void BlockUtils::insertionSortById(ParkingBlock** arr, int n) {
    for (int i = 1; i < n; i++) {
        ParkingBlock* key = arr[i];
        int j = i - 1;

        while (j >= 0 && strcmp(arr[j]->getId(), key->getId()) > 0) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

int BlockUtils::binarySearchById(ParkingBlock** arr, int n, const char* id) {
    int low = 0, high = n - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int comparison = strcmp(id, arr[mid]->getId());

        if (comparison == 0) return mid;

        if (comparison < 0)
            high = mid - 1;
        else
            low = mid + 1;
    }

    return -1;
}

static int partitionByOccupancy(ParkingBlock** arr, int low, int high) {
    int pivot = arr[high]->usedCount();
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j]->usedCount() >= pivot) {
            i++;
            genericSwap(arr[i], arr[j]);
        }
    }

    genericSwap(arr[i + 1], arr[high]);
    return i + 1;
}

void BlockUtils::quickSortByOccupancy(ParkingBlock** arr, int low, int high) {
    if (low < high) {
        int p = partitionByOccupancy(arr, low, high);

        quickSortByOccupancy(arr, low, p - 1);
        quickSortByOccupancy(arr, p + 1, high);
    }
}

// Custom hash table using separate chaining. No STL containers are used.
class VehicleIndex {class VehicleIndex {
    struct Entry { char vehicleNo[NAME_LEN]; int reservationId; Entry* next; };
    static const int BUCKETS = 101;
    Entry* table[BUCKETS];
    int count;
    unsigned int hashCode(const char* vehicleNo) const;
    Entry* find(const char* vehicleNo) const;
    VehicleIndex(const VehicleIndex&);
    VehicleIndex& operator=(const VehicleIndex&);
public:
    VehicleIndex();
    ~VehicleIndex();
    bool insert(const char* vehicleNo, int reservationId);
    bool contains(const char* vehicleNo) const;
    int  getReservationId(const char* vehicleNo) const;
    bool remove(const char* vehicleNo);
    int  size() const { return count; }
};

class ActivityLog {
    struct LogNode { char text[100]; LogNode* next; };
    LogNode* head;
    LogNode* tail;
    int count;
    ActivityLog(const ActivityLog&);
    ActivityLog& operator=(const ActivityLog&);
public:
    ActivityLog();
    ~ActivityLog();
    void log(const char* text);
    void display() const;
    int  size() const { return count; }
    const char* getAt(int index) const;
    bool saveToFile(const char* fileName) const;
    bool loadFromFile(const char* fileName);
};

class FineManager {
    int totalCollected;
    int finesIssued;
public:
    FineManager();

    int calculateFine(int minutesLate, int fineRate) const;
    int applyFine(const char* vehicleNo, int minutesLate, int fineRate, ActivityLog& log);
    int getTotalCollected() const { return totalCollected; }
    int getFinesIssued() const { return finesIssued; }
};

class AlertSystem {
public:
    bool checkCapacity(const ParkingBlock& block) const;
    void alertRepeatOffender(const char* vehicleNo, int noShows) const;
};

class ReportGenerator {
public:

    void printOccupancyReport(ParkingBlock** blocks, int n) const;
};

VehicleIndex::VehicleIndex() : count(0) {
    for (int i = 0; i < BUCKETS; ++i) table[i] = 0;
}
VehicleIndex::~VehicleIndex() {
    for (int i = 0; i < BUCKETS; ++i) {
        Entry* p = table[i];
        while (p) { Entry* old = p; p = p->next; delete old; }
    }
}
unsigned int VehicleIndex::hashCode(const char* vehicleNo) const {
    unsigned int hash = 5381;
    for (const unsigned char* p = (const unsigned char*)vehicleNo; *p; ++p)
        hash = ((hash << 5) + hash) + *p;
    return hash % BUCKETS;
}
VehicleIndex::Entry* VehicleIndex::find(const char* vehicleNo) const {
    if (!vehicleNo) return 0;
    unsigned int bucket = hashCode(vehicleNo);
    for (Entry* p = table[bucket]; p; p = p->next)
        if (strcmp(p->vehicleNo, vehicleNo) == 0) return p;
    return 0;
}
bool VehicleIndex::insert(const char* vehicleNo, int reservationId) {
    if (!vehicleNo || !vehicleNo[0] || count >= 200 || find(vehicleNo)) return false;
    unsigned int bucket = hashCode(vehicleNo);
    Entry* node = new Entry;
    copyStr(node->vehicleNo, vehicleNo, NAME_LEN);
    node->reservationId = reservationId;
    node->next = table[bucket];
    table[bucket] = node;
    ++count;
    return true;
}
bool VehicleIndex::contains(const char* vehicleNo) const { return find(vehicleNo) != 0; }
int VehicleIndex::getReservationId(const char* vehicleNo) const {
    Entry* p = find(vehicleNo);
    return p ? p->reservationId : -1;
}
bool VehicleIndex::remove(const char* vehicleNo) {
    if (!vehicleNo) return false;
    unsigned int bucket = hashCode(vehicleNo);
    Entry* p = table[bucket];
    Entry* previous = 0;
    while (p) {
        if (strcmp(p->vehicleNo, vehicleNo) == 0) {
            if (previous) previous->next = p->next; else table[bucket] = p->next;
            delete p; --count; return true;
        }
        previous = p; p = p->next;
    }
    return false;
}

ActivityLog::ActivityLog() : head(0), tail(0), count(0) {}

ActivityLog::~ActivityLog() {
    while (head) {
        LogNode* oldHead = head;
        head = head->next;
        delete oldHead;
    }
}

void ActivityLog::log(const char* text) {
    LogNode* newNode = new LogNode;
    copyStr(newNode->text, text, 100);
    newNode->next = 0;
    if (!tail) head = tail = newNode;
    else { tail->next = newNode; tail = newNode; }
    count++;
    saveToFile("activity_log.txt");
}

void ActivityLog::display() const {
    cout << "Activity log (" << count << " entries):" << endl;
    int number = 1;
    for (LogNode* p = head; p; p = p->next) cout << "  " << number++ << ". " << p->text << endl;
}
const char* ActivityLog::getAt(int index) const {
    if (index < 0) return 0;
    LogNode* p = head;
    while (p && index > 0) { p = p->next; --index; }
    return p ? p->text : 0;
}
bool ActivityLog::saveToFile(const char* fileName) const {
    if (!fileName || !fileName[0]) return false;
    ofstream file(fileName, ios::out | ios::trunc);
    if (!file) return false;
    int number = 1;
    for (LogNode* p = head; p; p = p->next)
        file << setw(4) << left << number++ << ". " << p->text << '\n';
    file.flush();
    bool good = file.good();
    file.close();
    return good;
}
bool ActivityLog::loadFromFile(const char* fileName) {
    if (!fileName || !fileName[0]) return false;
    ifstream file(fileName);
    if (!file) return false;
    char line[160];
    while (file.getline(line, sizeof(line))) {
        char* content = strchr(line, '.');
        if (content && content[1] == ' ') content += 2; else content = line;
        LogNode* node = new LogNode;
        copyStr(node->text, content, 100);
        node->next = 0;
        if (!tail) head = tail = node; else { tail->next = node; tail = node; }
        ++count;
    }
    return true;
}

FineManager::FineManager() : totalCollected(0), finesIssued(0) {}

int FineManager::calculateFine(int minutesLate, int fineRate) const {
    if (minutesLate <= 0 || fineRate <= 0) return 0;
    int blocksOf15 = (minutesLate + 14) / 15;
    return blocksOf15 * fineRate;
}

int FineManager::applyFine(const char* vehicleNo, int minutesLate, int fineRate, ActivityLog& log) {
    int fine = calculateFine(minutesLate, fineRate);
    if (fine > 0) {
        totalCollected += fine;
        finesIssued++;
        char line[100] = "FINE ";
        strncat(line, vehicleNo, 20);
        log.log(line);
    }
    return fine;
}

bool AlertSystem::checkCapacity(const ParkingBlock& block) const {
    if (block.usedCount() * 100 >= block.getCapacity() * 80) {
        cout << "[ALERT] Block " << block.getId() << " is " << block.usedCount() << "/" << block.getCapacity()
             << " full - consider redirecting new bookings." << endl;
        return true;
    }
    return false;
}
void AlertSystem::alertRepeatOffender(const char* vehicleNo, int noShows) const {
    cout << "[ALERT] Vehicle " << vehicleNo << " has " << noShows << " no-shows (repeat offender)." << endl;
}

void ReportGenerator::printOccupancyReport(ParkingBlock** blocks, int n) const {
    ParkingBlock* sortedCopy[MAX_BLOCKS];
    for (int i = 0; i < n; i++) sortedCopy[i] = blocks[i];
    BlockUtils::quickSortByOccupancy(sortedCopy, 0, n - 1);

    cout << "---- Occupancy report (most occupied first) ----" << endl;
    for (int i = 0; i < n; i++) {
        cout << "  Block " << sortedCopy[i]->getId() << "  used " << sortedCopy[i]->usedCount() << "/"
             << sortedCopy[i]->getCapacity() << "  (reserved " << sortedCopy[i]->countByState(SLOT_RESERVED)
             << ", occupied " << sortedCopy[i]->countByState(SLOT_OCCUPIED) << ")" << endl;
    }
}

enum ResStatus { RES_BOOKED, RES_CHECKED_IN, RES_EXPIRED };
class Reservation {
    static int counter;

    int id;
    char vehicleNo[NAME_LEN];
    char blockId[NAME_LEN];
    int slotIndex;
    int priority;
    ResStatus status;
public:
    Reservation();
    Reservation(const char* vehicleNo, const char* blockId, int slot, int priority);
    Reservation(const Reservation& other);
    Reservation& operator=(const Reservation& other);
    Reservation& setStatus(ResStatus s);
    int  getId() const { return id; }
    const char* getVehicleNo() const { return vehicleNo; }
    const char* getBlockId() const { return blockId; }
    int  getSlot() const { return slotIndex; }
    int  getPriority() const { return priority; }
    ResStatus getStatus() const { return status; }
    const char* statusName() const;
    static int totalCreated() { return counter; }
};

class ReservationList {
    struct Node { Reservation data; Node* prev; Node* next; };
    Node* head;
    Node* tail;
    int count;
    ReservationList(const ReservationList&);
    ReservationList& operator=(const ReservationList&);
public:
    ReservationList();
    ~ReservationList();
    void add(const Reservation& r);
    bool removeById(int id);
    Reservation* findById(int id);
    Reservation* findByVehicle(const char* vehicleNo);
    int  size() const { return count; }
    const Reservation* at(int index) const;
    void display() const;
};

struct WaitEntry {
    char vehicleNo[NAME_LEN];
    char blockId[NAME_LEN];
    int  priority;
};

class LinkedQueue {
    struct Node { WaitEntry data; Node* next; };
    Node* front;
    Node* rear;
    int count;
    LinkedQueue(const LinkedQueue&);
    LinkedQueue& operator=(const LinkedQueue&);
public:
    LinkedQueue();
    ~LinkedQueue();
    void enqueue(const WaitEntry& e);
    bool dequeue(WaitEntry& e);
    bool peek(WaitEntry& e) const;
    bool getAt(int index, WaitEntry& e) const;
    bool isEmpty() const { return count == 0; }
    int  size() const { return count; }
    void display() const;
};

class PriorityWaitlist {
    LinkedQueue levels[3];
public:
    void push(const char* vehicleNo, const char* blockId, int priority);
    bool pop(WaitEntry& e);
    bool peek(WaitEntry& e) const;
    bool getAt(int index, WaitEntry& e) const;
    bool isEmpty() const;
    int  size() const;
    void display() const;
};

class BlockMap {
    int adj[MAX_BLOCKS][MAX_BLOCKS];
    void dfsVisit(int node, int n, bool visited[], int order[], int& count) const;
public:
    BlockMap();
    void addEdge(int a, int b);
    bool isNeighbor(int a, int b) const { return a >= 0 && b >= 0 && a < MAX_BLOCKS && b < MAX_BLOCKS && adj[a][b] == 1; }
    int findNearestFreeBlock(int start, ParkingBlock** blocks, int n) const;
    int breadthFirst(int start, int n, int order[]) const;
    int depthFirst(int start, int n, int order[]) const;
    bool isReachable(int start, int target, int n) const;
    int chooseEmergencyBlock(int start, ParkingBlock** blocks, int n, int& distance,
                             int& freeSlots, int& score, int path[], int& pathLength) const;
    int adjacencyAt(int row, int col) const {
        return (row >= 0 && col >= 0 && row < MAX_BLOCKS && col < MAX_BLOCKS) ? adj[row][col] : 0;
    }
};

// Explainable rule-based AI wrapper: asks the graph for the best candidate.
// The heuristic is shortest route first, then more free slots, then lower occupancy.
class EmergencyAllocationAI {
public:
    int chooseBlock(const BlockMap& map, int entry, ParkingBlock** blocks, int n,
                    int& distance, int& freeSlots, int& score,
                    int path[], int& pathLength) const {
        return map.chooseEmergencyBlock(entry, blocks, n, distance, freeSlots, score, path, pathLength);
    }
    const char* modelType() const { return "Explainable rule-based decision engine"; }
};

class ReservationManager {
    ParkingBlock* blocks[MAX_BLOCKS];
    int blockCount;
    BlockMap map;
    EmergencyAllocationAI emergencyAI;
    ReservationList list;
    PriorityWaitlist waitlist;
    UndoStack undo;
    VehicleIndex& index;
    ActivityLog& log;

    int  placeBooking(const char* vehicleNo, int priority, int preferredIdx);
    void promoteFromWaitlist();
    int  blockIndexOf(const char* blockId) const;
    ReservationManager(const ReservationManager&);
    ReservationManager& operator=(const ReservationManager&);
public:
    ReservationManager(ParkingBlock** inputBlocks, int n, VehicleIndex& idx, ActivityLog& activityLog);
    void connectBlocks(const char* a, const char* b);

    int  makeReservation(const Vehicle& v, const char* preferredBlockId);
    int  makeEmergencyReservation(const Vehicle& v, const char* entryBlock,
                                  int path[], int& pathLength, int& chosenIndex,
                                  int& distance, int& freeSlots, int& score);
    int  bfsOrder(const char* startBlock, int order[]) const;
    int  dfsOrder(const char* startBlock, int order[]) const;
    bool blocksReachable(const char* from, const char* to) const;
    int  adjacencyAt(int row, int col) const { return map.adjacencyAt(row, col); }
    bool cancelReservation(int reservationId);
    bool undoLastReservation();
    bool checkIn(int reservationId);
    bool markNoShow(int reservationId, char* vehicleNoOut);

    void showReservations() const { list.display(); }
    void showWaitlist() const { waitlist.display(); }
    int  waitlistSize() const { return waitlist.size(); }
    ParkingBlock** getBlocks() { return blocks; }
    int  getBlockCount() const { return blockCount; }
    int  reservationCount() const { return list.size(); }
    const Reservation* reservationAt(int index) const { return list.at(index); }
    bool getWaitlistEntry(int index, WaitEntry& entry) const { return waitlist.getAt(index, entry); }
};

int Reservation::counter = 0;

Reservation::Reservation() : id(0), slotIndex(-1), priority(2), status(RES_BOOKED) {
    vehicleNo[0] = blockId[0] = '\0';
}

Reservation::Reservation(const char* vehicle, const char* block, int slot, int prio) {
    id = ++counter;
    copyStr(vehicleNo, vehicle, NAME_LEN);
    copyStr(blockId, block, NAME_LEN);
    slotIndex = slot;
    priority = prio;
    status = RES_BOOKED;
}

Reservation::Reservation(const Reservation& other) {
    id = other.id;
    copyStr(vehicleNo, other.vehicleNo, NAME_LEN);
    copyStr(blockId, other.blockId, NAME_LEN);
    slotIndex = other.slotIndex;
    priority = other.priority;
    status = other.status;
}
Reservation& Reservation::operator=(const Reservation& other) {
    if (this != &other) {
        id = other.id;
        copyStr(vehicleNo, other.vehicleNo, NAME_LEN);
        copyStr(blockId, other.blockId, NAME_LEN);
        slotIndex = other.slotIndex;
        priority = other.priority;
        status = other.status;
    }
    return *this;
}
Reservation& Reservation::setStatus(ResStatus s) { this->status = s; return *this; }
const char* Reservation::statusName() const {
    if (status == RES_BOOKED) return "BOOKED";
    if (status == RES_CHECKED_IN) return "CHECKED-IN";
    return "EXPIRED";
}

ReservationList::ReservationList() : head(0), tail(0), count(0) {}
ReservationList::~ReservationList() {
    while (head) {
        Node* oldHead = head;
        head = head->next;
        delete oldHead;
    }
}

void ReservationList::add(const Reservation& r) {
    Node* newNode = new Node;
    newNode->data = r;
    newNode->prev = tail;
    newNode->next = 0;
    if (tail) tail->next = newNode; else head = newNode;
    tail = newNode;
    count++;
}

bool ReservationList::removeById(int id) {
    for (Node* p = head; p; p = p->next) {
        if (p->data.getId() == id) {
            if (p->prev) p->prev->next = p->next; else head = p->next;
            if (p->next) p->next->prev = p->prev; else tail = p->prev;
            delete p;
            count--;
            return true;
        }
    }
    return false;
}
Reservation* ReservationList::findById(int id) {
    for (Node* p = head; p; p = p->next)
        if (p->data.getId() == id) return &p->data;
    return 0;
}
Reservation* ReservationList::findByVehicle(const char* vehicleNo) {
    for (Node* p = head; p; p = p->next)
        if (strcmp(p->data.getVehicleNo(), vehicleNo) == 0) return &p->data;
    return 0;
}
const Reservation* ReservationList::at(int index) const {
    if (index < 0) return 0;
    Node* p = head;
    while (p && index > 0) { p = p->next; index--; }
    return p ? &p->data : 0;
}
void ReservationList::display() const {
    cout << "Active reservations (" << count << "):" << endl;
    for (Node* p = head; p; p = p->next)
        cout << "  #" << p->data.getId() << "  " << p->data.getVehicleNo() << "  block "
             << p->data.getBlockId() << " slot " << p->data.getSlot() << "  "
             << p->data.statusName() << endl;
}

LinkedQueue::LinkedQueue() : front(0), rear(0), count(0) {}
LinkedQueue::~LinkedQueue() {
    while (front) {
        Node* oldFront = front;
        front = front->next;
        delete oldFront;
    }
}

void LinkedQueue::enqueue(const WaitEntry& e) {
    Node* newNode = new Node;
    newNode->data = e;
    newNode->next = 0;
    if (rear) rear->next = newNode; else front = newNode;
    rear = newNode;
    count++;
}

bool LinkedQueue::dequeue(WaitEntry& e) {
    if (!front) return false;
    Node* oldFront = front;
    e = oldFront->data;
    front = front->next;
    if (!front) rear = 0;
    delete oldFront;
    count--;
    return true;
}
bool LinkedQueue::peek(WaitEntry& e) const {
    if (!front) return false;
    e = front->data;
    return true;
}
bool LinkedQueue::getAt(int index, WaitEntry& e) const {
    if (index < 0) return false;
    Node* p = front;
    while (p && index > 0) { p = p->next; index--; }
    if (!p) return false;
    e = p->data;
    return true;
}
void LinkedQueue::display() const {
    for (Node* p = front; p; p = p->next)
        cout << p->data.vehicleNo << "(wants " << p->data.blockId << ") ";
}

void PriorityWaitlist::push(const char* vehicleNo, const char* blockId, int priority) {
    if (priority < 0) priority = 0;
    if (priority > 2) priority = 2;
    WaitEntry e;
    copyStr(e.vehicleNo, vehicleNo, NAME_LEN);
    copyStr(e.blockId, blockId, NAME_LEN);
    e.priority = priority;
    levels[priority].enqueue(e);
}

bool PriorityWaitlist::pop(WaitEntry& e) {
    for (int level = 0; level < 3; level++)
        if (levels[level].dequeue(e)) return true;
    return false;
}
bool PriorityWaitlist::peek(WaitEntry& e) const {
    for (int level = 0; level < 3; level++)
        if (levels[level].peek(e)) return true;
    return false;
}
bool PriorityWaitlist::getAt(int index, WaitEntry& e) const {
    if (index < 0) return false;
    for (int level = 0; level < 3; level++) {
        if (index < levels[level].size()) return levels[level].getAt(index, e);
        index -= levels[level].size();
    }
    return false;
}
bool PriorityWaitlist::isEmpty() const { return size() == 0; }
int PriorityWaitlist::size() const { return levels[0].size() + levels[1].size() + levels[2].size(); }
void PriorityWaitlist::display() const {
    const char* names[3] = { "Emergency", "Faculty  ", "Student  " };
    cout << "Waitlist (" << size() << "):" << endl;
    for (int level = 0; level < 3; level++) {
        cout << "  " << names[level] << ": ";
        levels[level].display();
        cout << endl;
    }
}

BlockMap::BlockMap() {
    for (int i = 0; i < MAX_BLOCKS; i++)
        for (int j = 0; j < MAX_BLOCKS; j++) adj[i][j] = 0;
}

void BlockMap::addEdge(int a, int b) {
    if (a < 0 || b < 0 || a >= MAX_BLOCKS || b >= MAX_BLOCKS) return;
    adj[a][b] = adj[b][a] = 1;
}

int BlockMap::breadthFirst(int start, int n, int order[]) const {
    if (start < 0 || start >= n || n > MAX_BLOCKS) return 0;
    bool visited[MAX_BLOCKS] = {false};
    int queue[MAX_BLOCKS], front = 0, rear = 0, count = 0;
    visited[start] = true; queue[rear++] = start;
    while (front < rear) {
        int u = queue[front++]; order[count++] = u;
        for (int v = 0; v < n; ++v)
            if (adj[u][v] && !visited[v]) { visited[v] = true; queue[rear++] = v; }
    }
    return count;
}
void BlockMap::dfsVisit(int node, int n, bool visited[], int order[], int& count) const {
    visited[node] = true; order[count++] = node;
    for (int v = 0; v < n; ++v)
        if (adj[node][v] && !visited[v]) dfsVisit(v, n, visited, order, count);
}
int BlockMap::depthFirst(int start, int n, int order[]) const {
    if (start < 0 || start >= n || n > MAX_BLOCKS) return 0;
    bool visited[MAX_BLOCKS] = {false}; int count = 0;
    dfsVisit(start, n, visited, order, count);
    return count;
}
bool BlockMap::isReachable(int start, int target, int n) const {
    if (start < 0 || target < 0 || start >= n || target >= n || n > MAX_BLOCKS) return false;
    int order[MAX_BLOCKS]; int count = breadthFirst(start, n, order);
    for (int i = 0; i < count; ++i) if (order[i] == target) return true;
    return false;
}
int BlockMap::findNearestFreeBlock(int start, ParkingBlock** blocks, int n) const {
    if (start < 0 || start >= n || n > MAX_BLOCKS) return -1;
    bool visited[MAX_BLOCKS] = {false}; int queue[MAX_BLOCKS], front = 0, rear = 0;
    visited[start] = true; queue[rear++] = start;
    while (front < rear) {
        int u = queue[front++];
        if (u != start && blocks[u]->hasFreeSlot()) return u;
        for (int v = 0; v < n; ++v)
            if (adj[u][v] && !visited[v]) { visited[v] = true; queue[rear++] = v; }
    }
    return -1;
}
int BlockMap::chooseEmergencyBlock(int start, ParkingBlock** blocks, int n, int& bestDistance,
                                   int& bestFreeSlots, int& bestScore, int path[], int& pathLength) const {
    bestDistance = -1; bestFreeSlots = 0; bestScore = -1; pathLength = 0;
    if (start < 0 || start >= n || n > MAX_BLOCKS) return -1;
    int distance[MAX_BLOCKS], parent[MAX_BLOCKS], queue[MAX_BLOCKS];
    for (int i = 0; i < n; ++i) { distance[i] = -1; parent[i] = -1; }
    int front = 0, rear = 0; queue[rear++] = start; distance[start] = 0;
    while (front < rear) {
        int u = queue[front++];
        for (int v = 0; v < n; ++v) {
            if (adj[u][v] && distance[v] == -1) {
                distance[v] = distance[u] + 1; parent[v] = u; queue[rear++] = v;
            }
        }
    }
    int best = -1;
    for (int i = 0; i < n; ++i) {
        if (distance[i] < 0 || !blocks[i]->hasFreeSlot()) continue;
        int freeNow = blocks[i]->countByState(SLOT_FREE);
        int usedPercent = (blocks[i]->usedCount() * 100) / blocks[i]->getCapacity();
        // Explainable heuristic: shortest path first, then more free spaces, then lower occupancy.
        int candidateScore = 10000 - distance[i] * 1000 + freeNow * 10 - usedPercent;
        if (best == -1 || distance[i] < bestDistance ||
            (distance[i] == bestDistance && freeNow > bestFreeSlots) ||
            (distance[i] == bestDistance && freeNow == bestFreeSlots && candidateScore > bestScore)) {
            best = i; bestDistance = distance[i]; bestFreeSlots = freeNow; bestScore = candidateScore;
        }
    }
    if (best != -1) {
        int reversePath[MAX_BLOCKS], len = 0, p = best;
        while (p != -1 && len < MAX_BLOCKS) { reversePath[len++] = p; p = parent[p]; }
        pathLength = len;
        for (int i = 0; i < len; ++i) path[i] = reversePath[len - 1 - i];
    }
    return best;
}

ReservationManager::ReservationManager(ParkingBlock** inputBlocks, int n, VehicleIndex& idx, ActivityLog& activityLog)
    : index(idx), log(activityLog) {
    if (n > MAX_BLOCKS) {
        blockCount = MAX_BLOCKS;
    } else {
        blockCount = n;
    }
    for (int i = 0; i < blockCount; i++) blocks[i] = inputBlocks[i];
    BlockUtils::insertionSortById(blocks, blockCount);
}

int ReservationManager::blockIndexOf(const char* blockId) const {
    return BlockUtils::binarySearchById(const_cast<ParkingBlock**>(blocks), blockCount, blockId);
}

void ReservationManager::connectBlocks(const char* a, const char* b) {
    int i = blockIndexOf(a), j = blockIndexOf(b);
    if (i != -1 && j != -1) map.addEdge(i, j);
}
int ReservationManager::bfsOrder(const char* startBlock, int order[]) const {
    return map.breadthFirst(blockIndexOf(startBlock), blockCount, order);
}
int ReservationManager::dfsOrder(const char* startBlock, int order[]) const {
    return map.depthFirst(blockIndexOf(startBlock), blockCount, order);
}
bool ReservationManager::blocksReachable(const char* from, const char* to) const {
    return map.isReachable(blockIndexOf(from), blockIndexOf(to), blockCount);
}
int ReservationManager::makeEmergencyReservation(const Vehicle& v, const char* entryBlock,
                                                  int path[], int& pathLength, int& chosenIndex,
                                                  int& distance, int& freeSlots, int& score) {
    if (v.getPriority() != 0) throw ParkingException("AI emergency allocation accepts emergency vehicles only");
    int start = blockIndexOf(entryBlock);
    if (start == -1) throw ParkingException("Invalid emergency entry block");
    if (index.contains(v.getNumber())) throw ParkingException("Double booking: vehicle already has a reservation");
    chosenIndex = emergencyAI.chooseBlock(map, start, blocks, blockCount, distance, freeSlots, score, path, pathLength);
    if (chosenIndex == -1) {
        waitlist.push(v.getNumber(), entryBlock, 0);
        index.insert(v.getNumber(), 0);
        char line[100] = "AI WAITLISTED EMERGENCY "; strncat(line, v.getNumber(), 20); log.log(line);
        throw ParkingException("No reachable free block. Emergency vehicle placed at the front of the priority waitlist.");
    }
    int id = placeBooking(v.getNumber(), 0, chosenIndex);
    char line[100] = "AI ASSIGNED EMERGENCY "; strncat(line, v.getNumber(), 20);
    strncat(line, " -> block ", 12); strncat(line, blocks[chosenIndex]->getId(), 10); log.log(line);
    return id;
}

int ReservationManager::placeBooking(const char* vehicleNo, int priority, int preferredIdx) {
    int target = preferredIdx;
    if (!blocks[target]->hasFreeSlot())
        target = map.findNearestFreeBlock(preferredIdx, blocks, blockCount);
    if (target == -1) return -1;

    int slot = blocks[target]->findFreeSlotIndex();
    blocks[target]->reserveSlot(slot);
    Reservation r(vehicleNo, blocks[target]->getId(), slot, priority);
    list.add(r);
    index.remove(vehicleNo);
    index.insert(vehicleNo, r.getId());
    undo.push(r.getId());

    char line[100] = "BOOKED ";
    strncat(line, vehicleNo, 20);
    strncat(line, " -> block ", 12);
    strncat(line, blocks[target]->getId(), 10);
    log.log(line);
    if (target != preferredIdx) log.log("  (redirected from full block)");
    return r.getId();
}

int ReservationManager::makeReservation(const Vehicle& v, const char* preferredBlockId) {
    int preferred = blockIndexOf(preferredBlockId);
    if (preferred == -1) throw ParkingException("Invalid block id");
    if (index.contains(v.getNumber())) throw ParkingException("Double booking: vehicle already has a reservation");

    int id = placeBooking(v.getNumber(), v.getPriority(), preferred);
    if (id == -1) {
        waitlist.push(v.getNumber(), preferredBlockId, v.getPriority());
        index.insert(v.getNumber(), 0);
        char line[100] = "WAITLISTED ";
        strncat(line, v.getNumber(), 20);
        log.log(line);
        throw ParkingException("No free slot nearby - request added to waitlist");
    }
    return id;
}

void ReservationManager::promoteFromWaitlist() {
    WaitEntry e;
    while (waitlist.peek(e)) {
        int preferred = blockIndexOf(e.blockId);
        if (preferred == -1) { waitlist.pop(e); continue; }
        if (placeBooking(e.vehicleNo, e.priority, preferred) == -1) break;
        waitlist.pop(e);
        char promotionMessage[100] = "WAITLIST PROMOTED ";
        strncat(promotionMessage, e.vehicleNo, 20);
        log.log(promotionMessage);
    }
}

bool ReservationManager::cancelReservation(int id) {
    Reservation* r = list.findById(id);
    if (!r) return false;
    int blockPos = blockIndexOf(r->getBlockId());
    blocks[blockPos]->releaseSlot(r->getSlot());
    index.remove(r->getVehicleNo());
    char line[100] = "CANCELLED ";
    strncat(line, r->getVehicleNo(), 20);
    log.log(line);
    list.removeById(id);
    promoteFromWaitlist();
    return true;
}

bool ReservationManager::undoLastReservation() {
    int id;
    while (undo.pop(id))
        if (list.findById(id)) return cancelReservation(id);
    return false;
}

bool ReservationManager::checkIn(int id) {
    Reservation* r = list.findById(id);
    if (!r || r->getStatus() != RES_BOOKED) return false;
    int blockPos = blockIndexOf(r->getBlockId());
    if (!blocks[blockPos]->occupySlot(r->getSlot())) return false;
    r->setStatus(RES_CHECKED_IN);
    char line[100] = "CHECK-IN ";
    strncat(line, r->getVehicleNo(), 20);
    log.log(line);
    return true;
}

bool ReservationManager::markNoShow(int id, char* vehicleNoOut) {
    Reservation* r = list.findById(id);
    if (!r || r->getStatus() != RES_BOOKED) return false;
    copyStr(vehicleNoOut, r->getVehicleNo(), NAME_LEN);
    int blockPos = blockIndexOf(r->getBlockId());
    blocks[blockPos]->expireSlot(r->getSlot());
    blocks[blockPos]->releaseSlot(r->getSlot());
    index.remove(vehicleNoOut);
    char line[100] = "NO-SHOW ";
    strncat(line, vehicleNoOut, 20);
    log.log(line);
    list.removeById(id);
    promoteFromWaitlist();
    return true;
}

static void tryReserve(ReservationManager& manager, const Vehicle& vehicle, const char* block) {
    try {
        int id = manager.makeReservation(vehicle, block);
        cout << vehicle.getNumber() << " (" << vehicle.getType() << ") wants " << block
             << " -> reservation #" << id << endl;
    } catch (ParkingException& e) {
        cout << vehicle.getNumber() << " wants " << block << " -> " << e.what() << endl;
    }
}

void autoDemoPankaj() {

    ParkingBlock a("A", 2), b("B", 2), c("C", 1);
    ParkingBlock* blocks[3] = { &c, &a, &b };
    VehicleIndex index;
    ActivityLog log;
    ReservationManager mgr(blocks, 3, index, log);
    mgr.connectBlocks("A", "B");
    mgr.connectBlocks("B", "C");
    FineManager fines;
    AlertSystem alerts;
    NoShowTree noShows;

    FourWheeler  f1("UK07-F1", 1), f2("UK07-F2", 1);
    TwoWheeler   s1("UK07-S1", 2), s2("UK07-S2", 2), s3("UK07-S3", 2);
    EmergencyVehicle amb("UK07-AMB");

    cout << "\n--- 1. Normal bookings, then overcrowding in block A ---" << endl;
    tryReserve(mgr, f1, "A");
    tryReserve(mgr, f2, "A");
    tryReserve(mgr, s1, "A");
    alerts.checkCapacity(a);

    cout << "\n--- 2. Error handling ---" << endl;
    tryReserve(mgr, f1, "A");
    tryReserve(mgr, s2, "Z");

    cout << "\n--- 3. Fill the campus, then waitlist ---" << endl;
    tryReserve(mgr, s2, "A");
    tryReserve(mgr, s3, "A");
    tryReserve(mgr, amb, "A");
    mgr.showWaitlist();

    cout << "\n--- 4. Cancel one booking: the emergency vehicle is served first ---" << endl;
    mgr.cancelReservation(1);
    mgr.showWaitlist();

    cout << "\n--- 5. Check-in, no-show and fine ---" << endl;
    mgr.checkIn(2);
    char vno[NAME_LEN];
    if (mgr.markNoShow(3, vno)) {
        noShows.recordNoShow(vno);
        int fine = fines.applyFine(vno, 40, s1.getFineRate(), log);
        cout << vno << " did not show up, fine = Rs " << fine << endl;
    }
    mgr.showWaitlist();

    cout << "\n--- 6. Undo last reservation (stack) ---" << endl;
    if (mgr.undoLastReservation()) {
        cout << "last reservation undone" << endl;
    } else {
        cout << "nothing to undo" << endl;
    }
    cout << "\n--- 7. Reports ---" << endl;
    mgr.showReservations();
    ReportGenerator().printOccupancyReport(mgr.getBlocks(), mgr.getBlockCount());
    noShows.display();
    cout << "Total fines collected: Rs " << fines.getTotalCollected() << endl;
    log.display();
}
static void appendText(char* out, int capacity, const char* format, ...);
