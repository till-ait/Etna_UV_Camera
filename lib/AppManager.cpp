#include "AppManager.h"
#include "UserInterface.h"
#include "InputHandler.h"
#include "CmdUi.h"
#include "CameraControler.h"

AppManager::AppManager() {
    std::cout << "App Manager created." << std::endl;
    is_running = true;
    Cameras = new std::vector<CameraControler*>();
    userInterface_ = new CmdUi(this);
    inputHandler_ = new InputHandler(this);

}

AppManager::~AppManager() {
    delete userInterface_;
    delete inputHandler_;

    for (CameraControler* cam : *Cameras)
    {
        delete cam;
    }
    Cameras->clear();
    delete Cameras;
    
    std::cout << "App Manager have been deleted." << std::endl;
}

bool AppManager::Start_App(void) {
    std::cout << "App Start ... " << std::endl;

    std::thread UI_Output_Thread([this]() { userInterface_->Update_Output(); });
    std::thread UI_Input_Thread([this]() { userInterface_->Update_Input(); });
    std::thread InputHandler_Thread([this]() { inputHandler_->Update(); });

    Cameras->push_back(new CameraControler((this), "cam330", "169.254.1.222"));
    Cameras->push_back(new CameraControler((this), "cam310", "169.254.1.248"));
    // if(Cameras->at(0)->Try_Connection()){
    //     // Cameras->at(0)->Acquire_Images();
    //     std::thread Camera1_Thread([this]() { Cameras->at(0)->Acquire_Images(); });
    //     Camera1_Thread.join();
    // }

    UI_Output_Thread.join();
    UI_Input_Thread.join();
    InputHandler_Thread.join();

    return true;
}


std::vector<CameraControler*>* AppManager::Get_Cameras(){
    return this->Cameras; 
}

// void AppManager::Set_Connected_Cameras(const std::vector<std::string> _connected_Cameras) {
//     this->connected_Cameras = _connected_Cameras;
// }


bool AppManager::Get_Is_Running() const {
    return is_running;
}


void AppManager::Set_Is_Running(bool value){
    is_running = value;
}

UserInterface* AppManager::Get_UserInterface() {
    return userInterface_;
}

InputHandler* AppManager::Get_InputHandler() {
    return inputHandler_;
}