#pragma once
#include <vhdf.hpp>

template <typename T>
class MemoryWrapper {
private:
    int disk;

    T block;

    bool blockNull = true;
    unsigned long long int blockId;

public:
    MemoryWrapper() {};
    explicit MemoryWrapper(int i) { disk = i; };
    int blockAccessCount = 0;

    void loadBlock(unsigned long long int index) {
        vhdf::readBlock(disk, index, &block);
        blockNull = false;
        blockId = index;
        blockAccessCount++;
    };

    void commitBlock() {
        if (!blockNull) vhdf::writeBlock(disk, blockId, &block);
        blockAccessCount++;
    }

    int getDiskId() {
        return disk;
    }

    size_t getLoadedBlockId() {
        if (!blockNull) return blockId;
        else return 0;
    }

    T* operator->() {return &block;};
};