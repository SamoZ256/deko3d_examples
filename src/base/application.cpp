#include "application.hpp"

ApplicationBase::ApplicationBase() {
    plInitialize(PlServiceType_User);
    romfsInit();
#ifdef DEBUG
    socketInitializeDefault();
    nxlinkStdio();
#endif

    // Initialize the controller
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    // Random
    srand(time(NULL));
}

ApplicationBase::~ApplicationBase() {
    plExit();
    romfsExit();
#ifdef DEBUG
    socketExit();
#endif
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
