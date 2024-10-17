#include "base/application.hpp"

#include <deko3d.hpp>

class TriangleApp : public Application {
public:
protected:
    void initialize() override {
    }

    void deinitialize() override {
    }

    void update() override {
    }

    void render() override {
    }
};

// Main program entrypoint
int main(int argc, char* argv[]) {
    TriangleApp app;
    app.run();

    return 0;
}
