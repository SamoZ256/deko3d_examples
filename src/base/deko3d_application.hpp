#pragma once

#include "application.hpp"
#include "memory_block.hpp"

#include <deko3d.hpp>

class Deko3DApplicationBase : public ApplicationBase {
protected:
    static constexpr u32 FRAMEBUFFER_COUNT = 2;
    static constexpr u32 FRAMEBUFFER_WIDTH = 1280;
    static constexpr u32 FRAMEBUFFER_HEIGHT = 720;
    static constexpr u32 STATIC_CMDBUF_SIZE = 0x1000;

    dk::UniqueDevice device;
    dk::UniqueQueue queue;

    MemoryBlock dataMemBlock;
    MemoryBlock imagesMemBlock;

    dk::Image framebuffers[FRAMEBUFFER_COUNT];
    DkCmdList framebufferCmdlists[FRAMEBUFFER_COUNT];

    dk::UniqueSwapchain swapchain;

    dk::UniqueCmdBuf mainCmdbuf;

    void initialize() override;
    void deinitialize() override;
    void update() override;

    virtual void initializeDeko3DObjects() = 0;
    virtual void deinitializeDeko3DObjects() = 0;
    virtual void render() = 0;

private:
    void createFramebuffers();
    void destroyFramebuffers();

protected:
    // Helpers
    void loadShader(MemoryAllocation& codeMem, dk::Shader& shader);
    void loadShader(const char* path, MemoryBlock& codeMemBlock, dk::Shader& shader);
    DkCmdList loadTexture(MemoryAllocation& scratchMem, MemoryAllocation& imageMem, dk::UniqueCmdBuf& cmdbuf, u32 width, u32 height, DkImageFormat format, dk::Image& image);
    DkCmdList loadTexture(const char* path, MemoryBlock& scratchMemBlock, MemoryBlock& imageMemBlock, dk::UniqueCmdBuf& cmdbuf, dk::Image& image);
};
