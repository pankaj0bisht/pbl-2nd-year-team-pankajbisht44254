
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
