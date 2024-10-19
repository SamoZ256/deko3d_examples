#include "memory_block.hpp"

DkGpuAddr MemoryAllocation::getGpuAddr() {
    return memBlock->getGpuAddr() + offset;
}

void* MemoryAllocation::getCpuAddr() {
    return (u8*)memBlock->getCpuAddr() + offset;
}

dk::MemBlock& MemoryAllocation::getNativeHandle() {
    return memBlock->getNativeHandle();
}

void MemoryBlock::initialize(dk::UniqueDevice& device, u32 size, int flags) {
    memBlock = dk::MemBlockMaker{device, align(size, (u32)DK_MEMBLOCK_ALIGNMENT)}.setFlags(flags).create();
}

void MemoryBlock::destroy() {
    memBlock.destroy();
}

MemoryAllocation MemoryBlock::allocate(u32 size, u32 alignment) {
    offset = align(offset, alignment);
    size = align(size, alignment);
    MemoryAllocation allocation{this, offset, size};

    offset += size;

    return allocation;
}
