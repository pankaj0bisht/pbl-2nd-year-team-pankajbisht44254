
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

// Custom hash table using separate chaining. No STL containers are used.
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
