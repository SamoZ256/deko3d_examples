#include "application.hpp"

Application::Application() {
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
}

Application::~Application() {
}

void Application::run() {
    initialize();

    while (appletMainLoop()) {
        padUpdate(&pad);

        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) {
            break;
        }

        update();
        render();
    }

    deinitialize();
}
