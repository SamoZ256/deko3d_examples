#include "base/deko3d_application.hpp"

#include <deko3d.hpp>

class TriangleApp : public Deko3DApplicationBase {
protected:
    void initializeDeko3DObjects() override {
        // Record the static command lists
        recordStaticCommands();
    }

    void deinitializeDeko3DObjects() override {
    }

    void render() override {
        // Run the main rendering command list
        queue.submitCommands(renderCmdlist);
    }

private:
    DkCmdList renderCmdlist;

    void recordStaticCommands() {
        // Set scissors
        cmdbuf.setScissors(0, { { 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT } });

        // Clear the color buffer
        cmdbuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);

        renderCmdlist = cmdbuf.finishList();
    }
};

// Main program entrypoint
int main(int argc, char* argv[]) {
    TriangleApp app;
    app.run();

    return 0;
}
