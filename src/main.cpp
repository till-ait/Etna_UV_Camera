#include <iostream>
#include "AppManager.h"
// #include "hal_UV_cam.h"
#include "hal_UV_Cam2.h"

int main(int argc, char** argv) {
    AppManager *appManager_;
    
    if(argc == 1) {
        appManager_ =new AppManager();
    }
    else {
        appManager_ =new AppManager(argv);
    }
    

    appManager_->Start_App();
    delete appManager_;

    return 0;
}