#include "base/application.hpp"

#include <iostream>

class HelloWorldApp : public ApplicationBase {
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
};

// Main program entrypoint
int main(int argc, char* argv[]) {
    HelloWorldApp app;
    app.run();

    return 0;
}
