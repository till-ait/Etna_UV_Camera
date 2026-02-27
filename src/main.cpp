#include <iostream>
#include "AppManager.h"
// #include "hal_UV_cam.h"
#include "hal_UV_Cam2.h"

int main() {
    std::cout << "Hello World !" << std::endl;

    AppManager appManager_;

    // appManager_.Start_App();
    main_eBus2();

    return 0;
}