#include "application.hpp"

ApplicationBase::ApplicationBase() {
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
}

ApplicationBase::~ApplicationBase() {
}

void ApplicationBase::run() {
    initialize();

    while (appletMainLoop()) {
        padUpdate(&pad);

        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) {
            break;
        }

        update();
    }

    deinitialize();
}
