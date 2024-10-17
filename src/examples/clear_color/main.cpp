#include "base/application.hpp"

#include <deko3d.hpp>

class ClearColorApp : public Application {
protected:
    void initialize() override {
        // Create the deko3d device
        device = dk::DeviceMaker{}.create();

        // Create the main queue
        queue = dk::QueueMaker{device}.setFlags(DkQueueFlags_Graphics).create();

        // Create the memory pools
        imagesMemblock = dk::MemBlockMaker{device, 16*1024*1024}.setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image).create();
        dataMemblock = dk::MemBlockMaker{device, 11*1024*1024}.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached).create();

        // Create the static command buffer and feed it freshly allocated memory
        cmdbuf = dk::CmdBufMaker{device}.create();
        cmdbuf.addMemory(dataMemblock, 0, STATIC_CMDBUF_SIZE);

        // Create the framebuffer resources
        createFramebuffers();
    }

    void deinitialize() override {
        destroyFramebufferResources();

        // Destroy the memory blocks
        dataMemblock.destroy();
        imagesMemblock.destroy();
    }

    void update() override {
    }

    void render() override {
        // Acquire a framebuffer from the swapchain (and wait for it to be available)
        int slot = queue.acquireImage(swapchain);

        // Run the command list that attaches said framebuffer to the queue
        queue.submitCommands(framebufferCmdlists[slot]);

        // Run the main rendering command list
        queue.submitCommands(renderCmdlist);

        // Now that we are done rendering, present it to the screen
        queue.presentImage(swapchain, slot);
    }

private:
    static constexpr unsigned FRAMEBUFFER_COUNT = 2;
    static constexpr uint32_t FRAMEBUFFER_WIDTH = 1280;
    static constexpr uint32_t FRAMEBUFFER_HEIGHT = 720;
    static constexpr unsigned STATIC_CMDBUF_SIZE = 0x1000;

    dk::UniqueDevice device;
    dk::UniqueQueue queue;

    dk::MemBlock imagesMemblock;
    dk::MemBlock dataMemblock;

    dk::UniqueCmdBuf cmdbuf;

    dk::Image framebuffers[FRAMEBUFFER_COUNT];
    DkCmdList framebufferCmdlists[FRAMEBUFFER_COUNT];
    dk::UniqueSwapchain swapchain;

    DkCmdList renderCmdlist;

    void createFramebuffers() {
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

        // Generate the main rendering cmdlist
        recordStaticCommands();
    }

    void destroyFramebufferResources() {
        // Return early if we have nothing to destroy
        if (!swapchain) return;

        // Make sure the queue is idle before destroying anything
        queue.waitIdle();

        // Clear the static cmdbuf, destroying the static cmdlists in the process
        cmdbuf.clear();

        // Destroy the swapchain
        swapchain.destroy();
    }

    void recordStaticCommands() {
        // Draw a scene using only scissors and clear colors
        cmdbuf.setScissors(0, { { 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT } });
        cmdbuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.25f, 0.0f, 1.0f);
        renderCmdlist = cmdbuf.finishList();
    }
};

// Main program entrypoint
int main(int argc, char* argv[]) {
    ClearColorApp app;
    app.run();

    return 0;
}
