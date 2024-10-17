#include "base/application.hpp"

#include <iostream>

class HelloWorldApp : public Application {
public:
protected:
    void initialize() override {
        consoleInit(NULL);

        printf("Hello, World!\n");
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
