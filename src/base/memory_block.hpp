#pragma once

#include "common.hpp"

#include <deko3d.hpp>

// TODO: Implement a proper memory allocator
class MemoryBlock {
public:
    void initialize(dk::UniqueDevice& device, u32 size, int flags);
    void destroy();

    u32 allocate(u32 size);

    void* getCpuAddr() {
        return memBlock.getCpuAddr();
    }

    // Getters
    dk::MemBlock& getMemBlock() { return memBlock; }

private:
    dk::MemBlock memBlock;

    u32 offset = 0;
};
