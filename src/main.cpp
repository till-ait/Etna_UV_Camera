#include <iostream>
#include "AppManager.h"
// #include "hal_UV_cam.h"
#include "hal_UV_Cam2.h"

int main(int argc, char** argv) {

    std::cout << "Have '" << argv[1] << "' arguments:\n";
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << "\n";
    }


    AppManager appManager_ = AppManager(argv);

    appManager_.Start_App();

    return 0;
}