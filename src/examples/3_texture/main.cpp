#include "base/deko3d_application.hpp"

#include <deko3d.hpp>

struct Image {
    dk::Image image;
    dk::ImageDescriptor* imageDescriptor;
};

class TextureApp : public Deko3DApplicationBase {
protected:
    void initializeDeko3DObjects() override {
        // Create memory blocks
        codeMemBlock.initialize(device, 128 * 1024 * 16, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code);
        scratchMemBlock.initialize(device, 16 * 1024 * 1024, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached);

        // Create shaders
        loadShader("romfs:/shaders/texture_vsh.dksh", codeMemBlock, vertexShader);
        loadShader("romfs:/shaders/texture_fsh.dksh", codeMemBlock, fragmentShader);

        // Upload vertex data
        vertexBufferMem = dataMemBlock.allocate(sizeof(VERTEX_DATA), alignof(Vertex));
        memcpy((u8*)vertexBufferMem.getCpuAddr(), VERTEX_DATA.data(), sizeof(VERTEX_DATA));

        // Temporary command buffer
        auto cmdbufMem = dataMemBlock.allocate(TMP_CMDBUF_SIZE, DK_CMDMEM_ALIGNMENT);
        dk::UniqueCmdBuf tmpCmdbuf = dk::CmdBufMaker{device}.create();
        tmpCmdbuf.addMemory(cmdbufMem.getNativeHandle(), cmdbufMem.offset, cmdbufMem.size);

        // Create textures
        //loadTexture(TODO);

        // Create samplers
        //samplerDescriptor->initialize(
        //    dk::Sampler{}
        //        .setFilter(DkFilter_Linear, DkFilter_Linear)
        //        .setWrapMode(DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge,
        //                     DkWrapMode_ClampToEdge));

        // Wait for the queue to be idle
        queue.waitIdle();

        // Free scratch memory
        scratchMemBlock.destroy();

        tmpCmdbuf.clear();

        // Record the static command lists
        recordStaticCommands();
    }

    void deinitializeDeko3DObjects() override {
        codeMemBlock.destroy();
    }

    void render() override {
        // Run the main rendering command list
        queue.submitCommands(renderCmdlist);
    }

private:
    static constexpr u32 TMP_CMDBUF_SIZE = 0x1000;

    struct Vertex
    {
        float position[3];
        float texCoord[2];
    };

    static constexpr std::array VERTEX_ATTRIB_STATE =
    {
        DkVtxAttribState{ 0, 0, offsetof(Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(Vertex, texCoord), DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
    };

    static constexpr std::array VERTEX_BUFFER_STATE =
    {
        DkVtxBufferState{ sizeof(Vertex), 0 },
    };

    static constexpr std::array VERTEX_DATA =
    {
        Vertex{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } },
        Vertex{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
        Vertex{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } },
        Vertex{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
    };

    MemoryBlock codeMemBlock;
    MemoryBlock scratchMemBlock;

    dk::Shader vertexShader;
    dk::Shader fragmentShader;

    MemoryAllocation vertexBufferMem;

    DkCmdList renderCmdlist;

    void recordStaticCommands() {
        dk::RasterizerState rasterizerState;
        dk::ColorState colorState;
        dk::ColorWriteState colorWriteState;

        // Configure viewport and scissor
        mainCmdbuf.setViewports(0, { { 0.0f, 0.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0.0f, 1.0f } });
        mainCmdbuf.setScissors(0, { { 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT } });

        // Clear the color buffer
        mainCmdbuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 0.0f);

        // Bind state
        mainCmdbuf.bindShaders(DkStageFlag_GraphicsMask, { &vertexShader, &fragmentShader });
        mainCmdbuf.bindRasterizerState(rasterizerState);
        mainCmdbuf.bindColorState(colorState);
        mainCmdbuf.bindColorWriteState(colorWriteState);
        mainCmdbuf.bindVtxBuffer(0, vertexBufferMem.getGpuAddr(), sizeof(VERTEX_DATA));
        mainCmdbuf.bindVtxAttribState(VERTEX_ATTRIB_STATE);
        mainCmdbuf.bindVtxBufferState(VERTEX_BUFFER_STATE);

        // Draw quad
        mainCmdbuf.draw(DkPrimitive_TriangleStrip, VERTEX_DATA.size(), 1, 0, 0);

        renderCmdlist = mainCmdbuf.finishList();
    }
};

// Main program entrypoint
int main(int argc, char* argv[]) {
    TextureApp app;
    app.run();

    return 0;
}
