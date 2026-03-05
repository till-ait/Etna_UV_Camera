#include <iostream>
#include "AppManager.h"
// #include "hal_UV_cam.h"
#include "hal_UV_Cam2.h"

int main(int argc, char** argv) {
    if(argc == 1) {
        std::cout << "Need argument cmd or qt to run" << std::endl;
        return 1;
    }
    
    AppManager appManager_ = AppManager(argv);

    appManager_.Start_App();

    return 0;
}