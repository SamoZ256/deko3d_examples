#pragma once

#include "common.hpp"

class ApplicationBase {
public:
    ApplicationBase();
    ~ApplicationBase();

    void run();

protected:
    PadState pad;

    virtual void initialize() = 0;
    virtual void deinitialize() = 0;
    virtual void update() = 0;
};
