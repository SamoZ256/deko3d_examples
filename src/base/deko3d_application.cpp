#include "deko3d_application.hpp"

void Deko3DApplicationBase::initialize() {
    // Create the deko3d device
    device = dk::DeviceMaker{}.create();

    // Create the main queue
    queue = dk::QueueMaker{device}.setFlags(DkQueueFlags_Graphics).create();

    // Create memory block for images
    dataMemblock = dk::MemBlockMaker{device, 11*1024*1024}.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached).create();
    imagesMemblock = dk::MemBlockMaker{device, 16*1024*1024}.setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image).create();

    // Create the static command buffer and feed it freshly allocated memory
    cmdbuf = dk::CmdBufMaker{device}.create();
    cmdbuf.addMemory(dataMemblock, 0, STATIC_CMDBUF_SIZE);

    createFramebuffers();

    // Initialize the host app objects
    initializeDeko3DObjects();
}

void Deko3DApplicationBase::deinitialize() {
    deinitializeDeko3DObjects();

    destroyFramebuffers();

    dataMemblock.destroy();
    imagesMemblock.destroy();
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
    uint64_t fbSize  = layoutFramebuffer.getSize();
    uint32_t fbAlign = layoutFramebuffer.getAlignment();
    fbSize = (fbSize + fbAlign - 1) & ~(fbAlign - 1);
    uint64_t offset = 0;
    for (unsigned i = 0; i < FRAMEBUFFER_COUNT; i++) {
        // Allocate a framebuffer
        framebuffers[i].initialize(layoutFramebuffer, imagesMemblock, offset);
        offset += fbSize;

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
