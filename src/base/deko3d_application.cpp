#include "deko3d_application.hpp"

#include <stdio.h>

void Deko3DApplicationBase::initialize() {
    // Create the deko3d device
    device = dk::DeviceMaker{}.create();

    // Create the main queue
    queue = dk::QueueMaker{device}.setFlags(DkQueueFlags_Graphics).create();

    // Create memory block for images
    dataMemBlock.initialize(device, 8 * 1024 * 1024, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached);
    imagesMemBlock.initialize(device, 16 * 1024 * 1024, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image);

    // Create the static command buffer and feed it freshly allocated memory
    cmdbuf = dk::CmdBufMaker{device}.create();
    cmdbuf.addMemory(dataMemBlock.getMemBlock(), dataMemBlock.allocate(STATIC_CMDBUF_SIZE), STATIC_CMDBUF_SIZE);

    createFramebuffers();

    // Initialize the host app objects
    initializeDeko3DObjects();
}

void Deko3DApplicationBase::deinitialize() {
    deinitializeDeko3DObjects();

    destroyFramebuffers();

    dataMemBlock.destroy();
    imagesMemBlock.destroy();
}

void Deko3DApplicationBase::update() {
    // Acquire a framebuffer
    int slot = queue.acquireImage(swapchain);

    // Bind framebuffer
    queue.submitCommands(framebufferCmdlists[slot]);

    render();

    // Present the framebuffer
    queue.presentImage(swapchain, slot);
}

void Deko3DApplicationBase::createFramebuffers() {
    // Create layout for the framebuffers
    dk::ImageLayout layoutFramebuffer;
    dk::ImageLayoutMaker{device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT)
        .initialize(layoutFramebuffer);

    // Create the framebuffers
    std::array<DkImage const*, FRAMEBUFFER_COUNT> fbArray;
    u64 fbSize  = layoutFramebuffer.getSize();
    u64 fbAlign = layoutFramebuffer.getAlignment();
    fbSize = align(fbSize, fbAlign);
    for (unsigned i = 0; i < FRAMEBUFFER_COUNT; i++) {
        // Allocate a framebuffer
        framebuffers[i].initialize(layoutFramebuffer, imagesMemBlock.getMemBlock(), imagesMemBlock.allocate(fbSize));

        // Generate a command list that binds it
        dk::ImageView colorTarget{ framebuffers[i] };
        cmdbuf.bindRenderTargets(&colorTarget);
        framebufferCmdlists[i] = cmdbuf.finishList();

        // Fill in the array for use later by the swapchain creation code
        fbArray[i] = &framebuffers[i];
    }

    // Create the swapchain using the framebuffers
    swapchain = dk::SwapchainMaker{device, nwindowGetDefault(), fbArray}.create();
}

void Deko3DApplicationBase::destroyFramebuffers() {
    // Wait for the queue to be idle
    queue.waitIdle();

    // Clear the command buffer (also destroys the command lists)
    cmdbuf.clear();

    swapchain.destroy();
}

void Deko3DApplicationBase::loadShader(const char *path, MemoryBlock& codeMemBlock, dk::Shader& shader) {
    // Load the shader from file
    FILE* f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    u32 size = ftell(f);
    rewind(f);

    size_t offset = codeMemBlock.allocate(align(size, (u32)DK_SHADER_CODE_ALIGNMENT));

    fread((char*)codeMemBlock.getCpuAddr() + offset, size, 1, f);
    fclose(f);

    // Create the shader
    dk::ShaderMaker(codeMemBlock.getMemBlock(), offset).initialize(shader);
}
