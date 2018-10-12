#include <vhdf.hpp>

template <typename T>
class MemoryWrapper {
private:
    int disk;

    T block;

    bool blockNull = true;
    unsigned long long int blockId;
public:
    explicit MemoryWrapper(int i) { disk = i; };

    void loadBlock(unsigned long long int index) {
        vhdf::readBlock(disk, index, &block);
        blockNull = false;
        blockId = index;
    };

    void commitBlock() {
        if (!blockNull) vhdf::writeBlock(disk, blockId, &block);
    }

    T* operator->() {return &block;};
};