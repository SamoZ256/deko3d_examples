#include "memory_block.hpp"

void MemoryBlock::initialize(dk::UniqueDevice& device, u32 size, int flags) {
    memBlock = dk::MemBlockMaker{device, size}.setFlags(flags).create();
}

void MemoryBlock::destroy() {
    memBlock.destroy();
}

u32 MemoryBlock::allocate(u32 size) {
    u32 oldOffset = offset;
    offset += size;

    return oldOffset;
}
