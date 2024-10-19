#pragma once

#include "common.hpp"

#include <deko3d.hpp>

struct MemoryAllocation {
    class MemoryBlock* memBlock;
    u32 offset;
    u32 size;

    DkGpuAddr getGpuAddr();

    void* getCpuAddr();

    // Getters
    dk::MemBlock& getNativeHandle();
};

class MemoryBlock {
public:
    void initialize(dk::UniqueDevice& device, u32 size, int flags);
    void destroy();

    MemoryAllocation allocate(u32 size, u32 alignment = 4); // TODO: what should the default alignment be?

    DkGpuAddr getGpuAddr() {
        return memBlock.getGpuAddr();
    }

    void* getCpuAddr() {
        return memBlock.getCpuAddr();
    }

    // Getters
    dk::MemBlock& getNativeHandle() { return memBlock; }

private:
    dk::MemBlock memBlock;

    u32 offset = 0;
};

// TODO: Implement a proper memory allocator
