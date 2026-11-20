
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
    int  getCapacity() const { return capacity; }
    SlotState getSlotStateAt(int index) const { return (index >= 0 && index < capacity) ? slots[index].getState() : SLOT_FREE; }
    int  findFreeSlotIndex() const;
    bool hasFreeSlot() const;
    int  countByState(SlotState s) const;
    int  usedCount() const;
    bool reserveSlot(int index);
    bool occupySlot(int index);
    bool expireSlot(int index);
    bool releaseSlot(int index);

    friend ostream& operator<<(ostream& out, const ParkingBlock& b);
};

namespace BlockUtils {
    void insertionSortById(ParkingBlock** arr, int n);
    int  binarySearchById(ParkingBlock** arr, int n, const char* id);
    void quickSortByOccupancy(ParkingBlock** arr, int low, int high);
}

ParkingSlot::ParkingSlot() : state(SLOT_FREE) {}
SlotState ParkingSlot::getState() const { return state; }
bool ParkingSlot::isFree() const { return state == SLOT_FREE; }
bool ParkingSlot::reserve()      { if (state != SLOT_FREE)     return false; state = SLOT_RESERVED; return true; }
bool ParkingSlot::markOccupied() { if (state != SLOT_RESERVED) return false; state = SLOT_OCCUPIED; return true; }
bool ParkingSlot::markExpired()  { if (state != SLOT_RESERVED) return false; state = SLOT_EXPIRED;  return true; }
bool ParkingSlot::release()      { state = SLOT_FREE; return true; }
const char* ParkingSlot::stateName() const {
    switch (state) {
        case SLOT_FREE:     return "FREE";
        case SLOT_RESERVED: return "RESERVED";
        case SLOT_OCCUPIED: return "OCCUPIED";
        default:            return "EXPIRED";
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
    for (int i = 0; i < capacity; i++) slots[i] = other.slots[i];
}

ParkingBlock& ParkingBlock::operator=(const ParkingBlock& other) {
    if (this == &other) return *this;
    delete[] slots;
    copyStr(id, other.id, NAME_LEN);
    capacity = other.capacity;
    slots = new ParkingSlot[capacity];
    for (int i = 0; i < capacity; i++) slots[i] = other.slots[i];
    return *this;
}
ParkingBlock::~ParkingBlock() { delete[] slots; }

int ParkingBlock::findFreeSlotIndex() const {
    for (int i = 0; i < capacity; i++)
        if (slots[i].isFree()) return i;
    return -1;
}
bool ParkingBlock::hasFreeSlot() const { return findFreeSlotIndex() != -1; }

int ParkingBlock::countByState(SlotState s) const {
    int total = 0;
    for (int i = 0; i < capacity; i++)
        if (slots[i].getState() == s) total++;
    return total;
}
int ParkingBlock::usedCount() const { return capacity - countByState(SLOT_FREE); }

bool ParkingBlock::reserveSlot(int index) { return (index >= 0 && index < capacity) && slots[index].reserve(); }
bool ParkingBlock::occupySlot(int index)  { return (index >= 0 && index < capacity) && slots[index].markOccupied(); }
bool ParkingBlock::expireSlot(int index)  { return (index >= 0 && index < capacity) && slots[index].markExpired(); }
bool ParkingBlock::releaseSlot(int index) { return (index >= 0 && index < capacity) && slots[index].release(); }

ostream& operator<<(ostream& out, const ParkingBlock& b) {
    out << "Block " << b.id << " [" << b.usedCount() << "/" << b.capacity << " used]  ";
    for (int i = 0; i < b.capacity; i++) out << "(" << i << ":" << b.slots[i].stateName() << ") ";
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
        if (comparison < 0) high = mid - 1; else low = mid + 1;
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

class VehicleIndex {
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

static void bubbleSort(int data[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        bool changed = false;
        for (int j = 0; j < n - i - 1; ++j)
            if (data[j] > data[j + 1]) { genericSwap(data[j], data[j + 1]); changed = true; }
        if (!changed) break;
    }
}
static void mergeValues(int data[], int temp[], int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) temp[k++] = data[i] <= data[j] ? data[i++] : data[j++];
    while (i <= mid) temp[k++] = data[i++];
    while (j <= right) temp[k++] = data[j++];
    for (int x = left; x <= right; ++x) data[x] = temp[x];
}
static void mergeSort(int data[], int temp[], int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(data, temp, left, mid); mergeSort(data, temp, mid + 1, right);
    mergeValues(data, temp, left, mid, right);
}
static void heapify(int data[], int n, int root) {
    int largest = root, left = 2 * root + 1, right = 2 * root + 2;
    if (left < n && data[left] > data[largest]) largest = left;
    if (right < n && data[right] > data[largest]) largest = right;
    if (largest != root) { genericSwap(data[root], data[largest]); heapify(data, n, largest); }
}
static void heapSort(int data[], int n) {
    for (int i = n / 2 - 1; i >= 0; --i) heapify(data, n, i);
    for (int i = n - 1; i > 0; --i) { genericSwap(data[0], data[i]); heapify(data, i, 0); }
}
static int linearSearch(const int data[], int n, int key) {
    for (int i = 0; i < n; ++i) if (data[i] == key) return i;
    return -1;
}

class OOPNumber {
    int value;
public:
    OOPNumber(int v = 0) : value(v) {}
    OOPNumber(const OOPNumber& other) : value(other.value) {}
    OOPNumber& operator=(const OOPNumber& other) { if (this != &other) value = other.value; return *this; }
    OOPNumber operator+(const OOPNumber& other) const { return OOPNumber(value + other.value); }
    OOPNumber operator-(const OOPNumber& other) const { return OOPNumber(value - other.value); }
    OOPNumber& operator++() { ++value; return *this; }
    OOPNumber operator++(int) { OOPNumber old(*this); value++; return old; }
    int getValue() const { return value; }
    friend ostream& operator<<(ostream& out, const OOPNumber& n) { out << n.value; return out; }
    friend istream& operator>>(istream& in, OOPNumber& n) { in >> n.value; return in; }
};
static OOPNumber addOOPObjects(OOPNumber a, OOPNumber b) { return a + b; }
static int calculateCharge(int minutes) { return minutes <= 0 ? 0 : ((minutes + 59) / 60) * 10; }
static int calculateCharge(int minutes, int hourlyRate) {
    return (minutes <= 0 || hourlyRate <= 0) ? 0 : ((minutes + 59) / 60) * hourlyRate;
}

class CampusPerson {
protected:
    char name[60];
public:
    CampusPerson(const char* n = "Campus user") { copyStr(name, n, 60); }
    virtual ~CampusPerson() {}
    virtual const char* role() const = 0;
    const char* getName() const { return name; }
};
class StudentRole : virtual public CampusPerson {
public:
    StudentRole(const char* n = "Student") : CampusPerson(n) {}
    const char* role() const { return "Student"; }
};
class FacultyRole : virtual public CampusPerson {
public:
    FacultyRole(const char* n = "Faculty") : CampusPerson(n) {}
    const char* role() const { return "Faculty"; }
};
class CampusCoordinator : public StudentRole, public FacultyRole {
public:
    CampusCoordinator(const char* n) : CampusPerson(n), StudentRole(n), FacultyRole(n) {}
    const char* role() const { return "Student-Faculty Coordinator"; }
};

struct RegisteredVehicle {
    Vehicle* vehicle;
    char owner[60];
};

static RegisteredVehicle gRegistry[200];
static int gRegistryCount = 0;
static ParkingBlock* gBlocks[MAX_BLOCKS];
static int gBlockCount = 0;
static VehicleIndex gVehicleIndex;
static ActivityLog gActivityLog;
static ReservationManager* gManager = 0;
static FineManager gFines;
static NoShowTree gNoShows;
struct AIDecision {
    bool made; char vehicleNumber[NAME_LEN]; char entryBlock[NAME_LEN]; char selectedBlock[NAME_LEN];
    int distance; int freeSlots; int score; int path[MAX_BLOCKS]; int pathLength;
};
static AIDecision gLastAI = {false, "", "", "", -1, 0, 0, {0}, 0};

static Vehicle* webFindVehicle(const char* number) {
    for (int i = 0; i < gRegistryCount; i++)
        if (strcmp(gRegistry[i].vehicle->getNumber(), number) == 0) return gRegistry[i].vehicle;
    return 0;
}
static const char* priorityName(int p) {
    if (p <= 0) return "Emergency";
    if (p == 1) return "Faculty";
    return "Student";
}
static const char* ownerFor(const char* number) {
    for (int i = 0; i < gRegistryCount; i++)
        if (strcmp(gRegistry[i].vehicle->getNumber(), number) == 0) return gRegistry[i].owner;
    return "—";
}

static void appendText(char* out, int capacity, const char* format, ...) {
    if (!out || capacity <= 0) return;
    int used = (int)strlen(out);
    if (used >= capacity - 1) return;
    va_list args;
    va_start(args, format);
    vsnprintf(out + used, (size_t)(capacity - used), format, args);
    va_end(args);
}
static int reserveWithExceptionAudit(ReservationManager& manager, const Vehicle& vehicle,
                                     const char* block) {

    return manager.makeReservation(vehicle, block);
}

static bool addRegisteredVehicle(const char* number, const char* owner, const char* type, const char* category, char* message, int messageSize) {
    if (!number || !number[0] || !owner || !owner[0]) { copyStr(message, "Vehicle number and owner are required", messageSize); return false; }
    if (strlen(number) >= NAME_LEN) { copyStr(message, "Vehicle number must be at most 15 characters", messageSize); return false; }
    if (webFindVehicle(number)) { copyStr(message, "This vehicle is already registered", messageSize); return false; }
    if (gRegistryCount >= 200) { copyStr(message, "Vehicle registry is full (200 vehicles)", messageSize); return false; }
    Vehicle* v = 0;
    int level = strcmp(category, "Faculty") == 0 ? 1 : 2;
    if (strcmp(type, "Emergency") == 0 || strcmp(category, "Emergency") == 0) v = new EmergencyVehicle(number);
    else if (strcmp(type, "Two Wheeler") == 0 || strcmp(type, "TwoWheeler") == 0) v = new TwoWheeler(number, level);
    else if (strcmp(type, "Four Wheeler") == 0 || strcmp(type, "FourWheeler") == 0) v = new FourWheeler(number, level);
    else { copyStr(message, "Choose a valid vehicle type", messageSize); return false; }
    gRegistry[gRegistryCount].vehicle = v;
    copyStr(gRegistry[gRegistryCount].owner, owner, 60);
    gRegistryCount++;
    copyStr(message, "Vehicle registered successfully", messageSize);
    return true;
}

static void htmlText(char* out, int capacity, const char* value) {
    if (!value) return;
    for (const unsigned char* p = (const unsigned char*)value; *p; ++p) {
        if (*p == '&') appendText(out, capacity, "&amp;");
        else if (*p == '<') appendText(out, capacity, "&lt;");
        else if (*p == '>') appendText(out, capacity, "&gt;");
        else if (*p == '"') appendText(out, capacity, "&quot;");
        else if (*p == '\'') appendText(out, capacity, "&#39;");
        else if (*p >= 32 || *p == '\n') appendText(out, capacity, "%c", *p);
    }
}
static void htmlCell(char* out, int capacity, const char* value) {
    appendText(out, capacity, "<td>"); htmlText(out, capacity, value); appendText(out, capacity, "</td>");
}
static void htmlStart(char* out, int capacity, const char* title, const char* message) {
    out[0] = '\0';
    appendText(out, capacity, "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>");
    htmlText(out, capacity, title);
    appendText(out, capacity, "</title></head><body><h1>Overparking Resolver</h1>");
    appendText(out, capacity, "<p>Smart Campus Parking Reservation, Redirection and Priority Waitlist System</p>");
    appendText(out, capacity, "<p><a href=\"/dashboard\">Dashboard</a> | <a href=\"/vehicles\">Vehicles</a> | <a href=\"/reservations\">Reservations</a> | <a href=\"/waitlist\">Waiting List</a> | <a href=\"/ai\">Emergency AI</a> | <a href=\"/graph\">Graph BFS/DFS</a> | <a href=\"/noshows\">No-Shows</a> | <a href=\"/reports\">Reports</a> | <a href=\"/logs\">Activity Log</a> | <a href=\"/lab\">DSA/OOP Lab</a></p><hr>");
    if (message && message[0]) {
        appendText(out, capacity, "<p><strong>Message: "); htmlText(out, capacity, message); appendText(out, capacity, "</strong></p><hr>");
    }
}
static void htmlEnd(char* out, int capacity) {
    appendText(out, capacity, "<hr><p>Phase 3 demonstration | C++ backend + plain HTML frontend | No CSS or JavaScript</p></body></html>");
}
static void renderVehicleTable(char* out, int capacity) {
    appendText(out, capacity, "<h2>Registered Vehicles</h2><table border=\"1\" cellpadding=\"5\"><tr><th>Vehicle Number</th><th>Owner</th><th>Type</th><th>Category</th></tr>");
    for (int i = 0; i < gRegistryCount; ++i) {
        appendText(out, capacity, "<tr>");
        htmlCell(out, capacity, gRegistry[i].vehicle->getNumber());
        htmlCell(out, capacity, gRegistry[i].owner);
        htmlCell(out, capacity, gRegistry[i].vehicle->getType());
        htmlCell(out, capacity, priorityName(gRegistry[i].vehicle->getPriority()));
        appendText(out, capacity, "</tr>");
    }
    appendText(out, capacity, "</table>");
    if (!gRegistryCount) appendText(out, capacity, "<p>No vehicles registered yet.</p>");
}
static void renderReservationTable(char* out, int capacity, bool withActions) {
    appendText(out, capacity, "<h2>Reservations</h2><table border=\"1\" cellpadding=\"5\"><tr><th>ID</th><th>Vehicle</th><th>Owner</th><th>Block</th><th>Slot</th><th>Priority</th><th>Status</th>");
    if (withActions) appendText(out, capacity, "<th>Action</th>");
    appendText(out, capacity, "</tr>");
    for (int i = 0; i < gManager->reservationCount(); ++i) {
        const Reservation* r = gManager->reservationAt(i); if (!r) continue;
        appendText(out, capacity, "<tr><td>%d</td>", r->getId());
        htmlCell(out, capacity, r->getVehicleNo()); htmlCell(out, capacity, ownerFor(r->getVehicleNo()));
        htmlCell(out, capacity, r->getBlockId()); appendText(out, capacity, "<td>%d</td>", r->getSlot());
        htmlCell(out, capacity, priorityName(r->getPriority())); htmlCell(out, capacity, r->statusName());
        if (withActions) {
            appendText(out, capacity, "<td><form method=\"post\" action=\"/reservation-action\"><input type=\"hidden\" name=\"reservationId\" value=\"%d\"><button name=\"action\" value=\"cancel\">Cancel</button> <button name=\"action\" value=\"checkin\">Check In</button> <button name=\"action\" value=\"noshow\">No-Show</button> <label>Minutes late <input name=\"minutesLate\" type=\"number\" min=\"0\" value=\"0\"></label></form></td>", r->getId());
        }
        appendText(out, capacity, "</tr>");
    }
    appendText(out, capacity, "</table>");
    if (!gManager->reservationCount()) appendText(out, capacity, "<p>No active reservations.</p>");
}
static void renderDashboard(char* out, int capacity) {
    int total=0, freeSlots=0, reserved=0, occupied=0;
    for (int i=0; i<gManager->getBlockCount(); ++i) {
        ParkingBlock* b=gManager->getBlocks()[i]; total+=b->getCapacity();
        freeSlots+=b->countByState(SLOT_FREE); reserved+=b->countByState(SLOT_RESERVED); occupied+=b->countByState(SLOT_OCCUPIED);
    }
    appendText(out,capacity,"<h2>Dashboard</h2><p><strong>Total slots:</strong> %d | <strong>Free:</strong> %d | <strong>Reserved:</strong> %d | <strong>Occupied:</strong> %d | <strong>Waiting:</strong> %d | <strong>Registered vehicles:</strong> %d</p>",total,freeSlots,reserved,occupied,gManager->waitlistSize(),gRegistryCount);
    appendText(out,capacity,"<h2>Parking Blocks</h2><table border=\"1\" cellpadding=\"5\"><tr><th>Block</th><th>Capacity</th><th>Free</th><th>Reserved</th><th>Occupied</th></tr>");
    for(int i=0;i<gManager->getBlockCount();++i){ ParkingBlock* b=gManager->getBlocks()[i]; appendText(out,capacity,"<tr>"); htmlCell(out,capacity,b->getId()); appendText(out,capacity,"<td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>",b->getCapacity(),b->countByState(SLOT_FREE),b->countByState(SLOT_RESERVED),b->countByState(SLOT_OCCUPIED)); }
    appendText(out,capacity,"</table>");
    for (int i = 0; i < gManager->getBlockCount(); ++i) {
        ParkingBlock* block = gManager->getBlocks()[i];
        if (block->usedCount() * 100 >= block->getCapacity() * 80) {
            appendText(out, capacity, "<p><strong>CAPACITY ALERT:</strong> Block ");
            htmlText(out, capacity, block->getId());
            appendText(out, capacity, " is at least 80%% full.</p>");
        }
    }
    renderReservationTable(out,capacity,false);
}
static void renderVehicles(char* out, int capacity) {
    appendText(out,capacity,"<h2>Register Vehicle</h2><form method=\"post\" action=\"/register-vehicle\"><p>Vehicle number (max 15 characters): <input name=\"vehicleNumber\" maxlength=\"15\" required></p><p>Owner name: <input name=\"owner\" maxlength=\"59\" required></p><p>Vehicle type: <select name=\"type\"><option>Two Wheeler</option><option>Four Wheeler</option><option>Emergency</option></select></p><p>Category: <select name=\"category\"><option>Student</option><option>Faculty</option><option>Emergency</option></select></p><button type=\"submit\">Register Vehicle</button></form>");
    renderVehicleTable(out,capacity);
}
static void renderReservations(char* out, int capacity) {
    appendText(out,capacity,"<h2>Make Reservation</h2><p>Register the vehicle first on the Vehicles page.</p><form method=\"post\" action=\"/make-reservation\"><p>Vehicle number: <input name=\"vehicleNumber\" maxlength=\"15\" required></p><p>Preferred block: <select name=\"blockId\"><option>A</option><option>B</option><option>C</option></select></p><button type=\"submit\">Reserve Slot</button></form>");
    appendText(out,capacity,"<h2>Undo</h2><form method=\"post\" action=\"/reservation-action\"><button name=\"action\" value=\"undo\">Undo Last Reservation</button></form>");
    renderReservationTable(out,capacity,true);
}
static void renderWaitlist(char* out, int capacity) {
    appendText(out,capacity,"<h2>Priority Waiting List</h2><p>Priority: Emergency first, then Faculty, then Student. FIFO order is maintained within each priority.</p><table border=\"1\" cellpadding=\"5\"><tr><th>Position</th><th>Vehicle</th><th>Requested Block</th><th>Priority</th></tr>");
    for(int i=0;i<gManager->waitlistSize();++i){ WaitEntry e; if(!gManager->getWaitlistEntry(i,e)) continue; appendText(out,capacity,"<tr><td>%d</td>",i+1); htmlCell(out,capacity,e.vehicleNo); htmlCell(out,capacity,e.blockId); htmlCell(out,capacity,priorityName(e.priority)); appendText(out,capacity,"</tr>"); }
    appendText(out,capacity,"</table>"); if(!gManager->waitlistSize()) appendText(out,capacity,"<p>The waiting list is empty.</p>");
}
static void renderNoShows(char* out, int capacity) {
    appendText(out,capacity,"<h2>No-Show Register (BST Inorder)</h2><table border=\"1\" cellpadding=\"5\"><tr><th>Vehicle</th><th>No-Show Count</th><th>Record</th></tr>");
    bool any=false; for(int i=0;;++i){char number[NAME_LEN];int count=0;if(!gNoShows.getEntryAt(i,number,NAME_LEN,count))break;any=true;appendText(out,capacity,"<tr>");htmlCell(out,capacity,number);appendText(out,capacity,"<td>%d</td>",count);htmlCell(out,capacity,count>=3?"Repeat offender":"Normal");appendText(out,capacity,"</tr>");}
    appendText(out,capacity,"</table>"); if(!any) appendText(out,capacity,"<p>No no-show records yet.</p>");
}
static void renderReports(char* out, int capacity) {
    int total=0,freeSlots=0,reserved=0,occupied=0; for(int i=0;i<gManager->getBlockCount();++i){ParkingBlock*b=gManager->getBlocks()[i];total+=b->getCapacity();freeSlots+=b->countByState(SLOT_FREE);reserved+=b->countByState(SLOT_RESERVED);occupied+=b->countByState(SLOT_OCCUPIED);}
    appendText(out,capacity,"<h2>Occupancy Report</h2><p>Total: %d | Free: %d | Reserved: %d | Occupied: %d</p><p>Total fines collected: Rs %d | Fines issued: %d</p>",total,freeSlots,reserved,occupied,gFines.getTotalCollected(),gFines.getFinesIssued());
    appendText(out,capacity,"<table border=\"1\" cellpadding=\"5\"><tr><th>Block</th><th>Capacity</th><th>Used</th><th>Free</th><th>Occupancy percent</th></tr>");
    for(int i=0;i<gManager->getBlockCount();++i){ParkingBlock*b=gManager->getBlocks()[i];int pct=b->getCapacity()?b->usedCount()*100/b->getCapacity():0;appendText(out,capacity,"<tr>");htmlCell(out,capacity,b->getId());appendText(out,capacity,"<td>%d</td><td>%d</td><td>%d</td><td>%d%%</td></tr>",b->getCapacity(),b->usedCount(),b->countByState(SLOT_FREE),pct);} appendText(out,capacity,"</table>");
}
static void renderAI(char* out, int capacity) {
    appendText(out,capacity,"<h2>Automatic Emergency Vehicle Allocation</h2><p>The C++ decision engine selects a reachable free block. It gives preference to shortest route, then more free spaces, then lower occupancy.</p><form method=\"post\" action=\"/emergency-allocate\"><p>Emergency vehicle number: <input name=\"vehicleNumber\" maxlength=\"15\" required></p><p>Owner / Department: <input name=\"owner\" maxlength=\"59\" required></p><p>Campus entry block: <select name=\"entryBlock\"><option>A</option><option>B</option><option>C</option></select></p><button type=\"submit\">Allocate Emergency Parking</button></form>");
    if(gLastAI.made){appendText(out,capacity,"<h3>Last AI Decision</h3><p>Vehicle: ");htmlText(out,capacity,gLastAI.vehicleNumber);appendText(out,capacity," | Entry: ");htmlText(out,capacity,gLastAI.entryBlock);appendText(out,capacity," | Selected block: ");htmlText(out,capacity,gLastAI.selectedBlock);appendText(out,capacity," | Distance: %d edges | Free slots at decision: %d</p><p>Route: ",gLastAI.distance,gLastAI.freeSlots);for(int i=0;i<gLastAI.pathLength;++i){if(i)appendText(out,capacity," &rarr; ");htmlText(out,capacity,gManager->getBlocks()[gLastAI.path[i]]->getId());}appendText(out,capacity,"</p>");}
    appendText(out,capacity,"<p><strong>AI type:</strong> Explainable rule-based decision engine (not a trained machine-learning model).</p>");
}
static void renderGraph(char* out, int capacity, const char* startBlock) {
    appendText(out,capacity,"<h2>Campus Graph: Adjacency Matrix, BFS and DFS</h2><form method=\"get\" action=\"/graph\"><label>Start block: <select name=\"start\"><option>A</option><option>B</option><option>C</option></select></label> <button type=\"submit\">Show Traversals</button></form><h3>Adjacency Matrix</h3><table border=\"1\" cellpadding=\"5\"><tr><th>Block</th>");
    for(int c=0;c<gManager->getBlockCount();++c){appendText(out,capacity,"<th>");htmlText(out,capacity,gManager->getBlocks()[c]->getId());appendText(out,capacity,"</th>");}appendText(out,capacity,"</tr>");
    for(int r=0;r<gManager->getBlockCount();++r){appendText(out,capacity,"<tr><th>");htmlText(out,capacity,gManager->getBlocks()[r]->getId());appendText(out,capacity,"</th>");for(int c=0;c<gManager->getBlockCount();++c)appendText(out,capacity,"<td>%d</td>",gManager->adjacencyAt(r,c));appendText(out,capacity,"</tr>");}appendText(out,capacity,"</table>");
    int bfs[MAX_BLOCKS],dfs[MAX_BLOCKS];int bc=gManager->bfsOrder(startBlock,bfs),dc=gManager->dfsOrder(startBlock,dfs);
    appendText(out,capacity,"<h3>BFS starting from ");htmlText(out,capacity,startBlock);appendText(out,capacity,"</h3><p>");for(int i=0;i<bc;++i){if(i)appendText(out,capacity," &rarr; ");htmlText(out,capacity,gManager->getBlocks()[bfs[i]]->getId());}if(!bc)appendText(out,capacity,"Invalid start block");appendText(out,capacity,"</p><h3>DFS starting from ");htmlText(out,capacity,startBlock);appendText(out,capacity,"</h3><p>");for(int i=0;i<dc;++i){if(i)appendText(out,capacity," &rarr; ");htmlText(out,capacity,gManager->getBlocks()[dfs[i]]->getId());}if(!dc)appendText(out,capacity,"Invalid start block");appendText(out,capacity,"</p><p>Matrix stores connections; BFS and DFS visit the graph.</p>");
}
static void renderLogs(char* out, int capacity) {
    appendText(out,capacity,"<h2>Activity Log</h2><form method=\"post\" action=\"/save-log\"><button type=\"submit\">Save Activity Log</button></form><ol>");
    for(int i=0;i<gActivityLog.size();++i){appendText(out,capacity,"<li>");htmlText(out,capacity,gActivityLog.getAt(i));appendText(out,capacity,"</li>");}appendText(out,capacity,"</ol>");if(!gActivityLog.size())appendText(out,capacity,"<p>No activity recorded.</p>");
}
static void renderLab(char* out, int capacity) {
    const int source[8]={42,8,19,3,27,11,6,31};int bubble[8],merge[8],heap[8],scratch[8];for(int i=0;i<8;++i)bubble[i]=merge[i]=heap[i]=source[i];bubbleSort(bubble,8);mergeSort(merge,scratch,0,7);heapSort(heap,8);
    appendText(out,capacity,"<h2>DSA and OOP Demonstration Lab</h2><h3>Sorting (same sample values)</h3><p>Original: 42, 8, 19, 3, 27, 11, 6, 31</p>");
    appendText(out,capacity,"<p>Bubble Sort: ");for(int i=0;i<8;++i)appendText(out,capacity,"%d%s",bubble[i],i==7?"":" , ");appendText(out,capacity,"</p><p>Merge Sort: ");for(int i=0;i<8;++i)appendText(out,capacity,"%d%s",merge[i],i==7?"":" , ");appendText(out,capacity,"</p><p>Heap Sort: ");for(int i=0;i<8;++i)appendText(out,capacity,"%d%s",heap[i],i==7?"":" , ");appendText(out,capacity,"</p>");
    OOPNumber first(12),second(8);CampusCoordinator coordinator("Sample Campus Coordinator");appendText(out,capacity,"<p>Linear search for 19 in the original list returns index: %d</p><h3>OOP Demonstrations</h3><p>Operator + result (12 + 8): %d</p><p>Function returning objects result: %d</p><p>Operator - result (12 - 8): %d</p><p>Overloaded parking charge, default rate: Rs %d</p><p>Overloaded parking charge at Rs 30/hour: Rs %d</p><p>Hybrid inheritance role: ", linearSearch(source,8,19),(first+second).getValue(),addOOPObjects(first,second).getValue(),(first-second).getValue(),calculateCharge(75),calculateCharge(75,30));htmlText(out,capacity,coordinator.role());appendText(out,capacity,"</p><p>Other implemented concepts include classes, objects, constructors, destructors, copy constructors, templates, friend functions, virtual functions, abstract classes, inheritance, exception handling and file handling.</p><p><a href=\"/graph\">Open the adjacency matrix and graph traversal page</a>.</p>");
}
static void renderHtmlPage(char* out, int capacity, const char* page, const char* message, const char* graphStart) {
    const char* title="Dashboard";if(strcmp(page,"vehicles")==0)title="Vehicles";else if(strcmp(page,"reservations")==0)title="Reservations";else if(strcmp(page,"waitlist")==0)title="Waiting List";else if(strcmp(page,"ai")==0)title="Emergency AI";else if(strcmp(page,"graph")==0)title="Graph BFS and DFS";else if(strcmp(page,"noshows")==0)title="No-Show Register";else if(strcmp(page,"reports")==0)title="Reports";else if(strcmp(page,"logs")==0)title="Activity Log";else if(strcmp(page,"lab")==0)title="DSA and OOP Lab";
    htmlStart(out,capacity,title,message);
    if(strcmp(page,"vehicles")==0)renderVehicles(out,capacity);else if(strcmp(page,"reservations")==0)renderReservations(out,capacity);else if(strcmp(page,"waitlist")==0)renderWaitlist(out,capacity);else if(strcmp(page,"ai")==0)renderAI(out,capacity);else if(strcmp(page,"graph")==0)renderGraph(out,capacity,graphStart&&graphStart[0]?graphStart:"A");else if(strcmp(page,"noshows")==0)renderNoShows(out,capacity);else if(strcmp(page,"reports")==0)renderReports(out,capacity);else if(strcmp(page,"logs")==0)renderLogs(out,capacity);else if(strcmp(page,"lab")==0)renderLab(out,capacity);else renderDashboard(out,capacity);
    htmlEnd(out,capacity);
}
static void decodeFormPart(const char* input, int length, char* output, int outSize) {
    int outputIndex = 0;

    for (int i = 0; i < length && outputIndex < outSize - 1; ++i) {
        if (input[i] == '+') {
            output[outputIndex++] = ' ';
        } else if (input[i] == '%' && i + 2 < length) {
            char hex[3] = { input[i + 1], input[i + 2], '\0' };
            output[outputIndex++] = (char)strtol(hex, 0, 16);
            i += 2;
        } else {
            output[outputIndex++] = input[i];
        }
    }

    output[outputIndex] = '\0';
}
