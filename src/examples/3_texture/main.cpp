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
        u8 textureData[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
        for (u32 y = 0; y < TEXTURE_HEIGHT; y++) {
            for (u32 x = 0; x < TEXTURE_WIDTH; x++) {
                textureData[y][x][0] = (x * 255) / (TEXTURE_WIDTH - 1);
                textureData[y][x][1] = (y * 255) / (TEXTURE_HEIGHT - 1);
                textureData[y][x][2] = rand() % 256;
                textureData[y][x][3] = 255;
            }
        }
        loadTexture(textureData, scratchMemBlock, imagesMemBlock, tmpCmdbuf, TEXTURE_WIDTH, TEXTURE_HEIGHT, DkImageFormat_RGBA8_Unorm, image.image);
        auto imageDescriptorMem = dataMemBlock.allocate(sizeof(dk::ImageDescriptor), alignof(dk::ImageDescriptor));
        image.imageDescriptor = static_cast<dk::ImageDescriptor*>(imageDescriptorMem.getCpuAddr());
        image.imageDescriptor->initialize(image.image);

        // Create samplers
        auto samplerDescriptorMem = dataMemBlock.allocate(sizeof(dk::SamplerDescriptor), alignof(dk::SamplerDescriptor));
        samplerDescriptor = static_cast<dk::SamplerDescriptor*>(samplerDescriptorMem.getCpuAddr());
        samplerDescriptor->initialize(
            dk::Sampler{}
                .setFilter(DkFilter_Linear, DkFilter_Linear)
                .setWrapMode(DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge,
                             DkWrapMode_ClampToEdge));

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
    static constexpr u32 TEXTURE_WIDTH = 8;
    static constexpr u32 TEXTURE_HEIGHT = 8;

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

    Image image;
    dk::SamplerDescriptor* samplerDescriptor;

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
