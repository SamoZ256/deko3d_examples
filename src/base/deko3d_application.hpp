#pragma once

#include "application.hpp"

#include <deko3d.hpp>

class Deko3DApplicationBase : public ApplicationBase {
protected:
    static constexpr unsigned FRAMEBUFFER_COUNT = 2;
    static constexpr uint32_t FRAMEBUFFER_WIDTH = 1280;
    static constexpr uint32_t FRAMEBUFFER_HEIGHT = 720;
    static constexpr unsigned STATIC_CMDBUF_SIZE = 0x1000;

    dk::UniqueDevice device;
    dk::UniqueQueue queue;

    dk::MemBlock dataMemBlock;
    dk::MemBlock imagesMemBlock;

    dk::Image framebuffers[FRAMEBUFFER_COUNT];
    DkCmdList framebufferCmdlists[FRAMEBUFFER_COUNT];

    dk::UniqueSwapchain swapchain;

    dk::UniqueCmdBuf cmdbuf;

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
    void loadShader(const char *path, DkMemBlock codeMemBlock, u32 memOffset, dk::Shader& shader, u32& codeSize);
};
