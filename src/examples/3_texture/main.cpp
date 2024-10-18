#include "base/deko3d_application.hpp"

#include <deko3d.hpp>

class TextureApp : public Deko3DApplicationBase {
protected:
    void initializeDeko3DObjects() override {
        // Create memory blocks
        codeMemBlock.initialize(device, 128 * 1024 * 16, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code);

        // Create shaders
        loadShader("romfs:/shaders/main_vsh.dksh", codeMemBlock, vertexShader);
        loadShader("romfs:/shaders/main_fsh.dksh", codeMemBlock, fragmentShader);

        // Upload vertex data
        vertexBufferMem = dataMemBlock.allocate(sizeof(VERTEX_DATA), alignof(Vertex));
        memcpy((u8*)vertexBufferMem.getCpuAddr(), VERTEX_DATA.data(), sizeof(VERTEX_DATA));

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
    struct Vertex
    {
        float position[3];
        float color[3];
    };

    static constexpr std::array VERTEX_ATTRIB_STATE =
    {
        DkVtxAttribState{ 0, 0, offsetof(Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(Vertex, color),    DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
    };

    static constexpr std::array VERTEX_BUFFER_STATE =
    {
        DkVtxBufferState{ sizeof(Vertex), 0 },
    };

    static constexpr std::array VERTEX_DATA =
    {
        Vertex{ {  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        Vertex{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        Vertex{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    };

    MemoryBlock codeMemBlock;

    dk::Shader vertexShader;
    dk::Shader fragmentShader;

    MemoryAllocation vertexBufferMem;

    DkCmdList renderCmdlist;

    void recordStaticCommands() {
        dk::RasterizerState rasterizerState;
        dk::ColorState colorState;
        dk::ColorWriteState colorWriteState;

        // Configure viewport and scissor
        cmdbuf.setViewports(0, { { 0.0f, 0.0f, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0.0f, 1.0f } });
        cmdbuf.setScissors(0, { { 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT } });

        // Clear the color buffer
        cmdbuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 0.0f);

        // Bind state required for drawing the triangle
        cmdbuf.bindShaders(DkStageFlag_GraphicsMask, { &vertexShader, &fragmentShader });
        cmdbuf.bindRasterizerState(rasterizerState);
        cmdbuf.bindColorState(colorState);
        cmdbuf.bindColorWriteState(colorWriteState);
        cmdbuf.bindVtxBuffer(0, vertexBufferMem.getGpuAddr(), sizeof(VERTEX_DATA));
        cmdbuf.bindVtxAttribState(VERTEX_ATTRIB_STATE);
        cmdbuf.bindVtxBufferState(VERTEX_BUFFER_STATE);

        // Draw the triangle
        cmdbuf.draw(DkPrimitive_Triangles, VERTEX_DATA.size(), 1, 0, 0);

        renderCmdlist = cmdbuf.finishList();
    }
};

// Main program entrypoint
int main(int argc, char* argv[]) {
    TextureApp app;
    app.run();

    return 0;
}
