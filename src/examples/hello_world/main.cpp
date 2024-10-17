#include "base/application.hpp"

#include <iostream>

class HelloWorldApp : public Application {
protected:
    void initialize() override {
        consoleInit(NULL);

        std::cout << "Hello, World!" << std::endl;
    }

    void deinitialize() override {
        consoleExit(NULL);
    }

    void update() override {
        consoleUpdate(NULL);
    }

    void render() override {
    }
};

// Main program entrypoint
int main(int argc, char* argv[]) {
    HelloWorldApp app;
    app.run();

    return 0;
}
