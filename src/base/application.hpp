#pragma once

#include <switch.h>

class Application {
public:
    Application();
    ~Application();

    void run();

protected:
    PadState pad;

    virtual void initialize() = 0;
    virtual void deinitialize() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
};
