#include "AppManager.h"
#include "UserInterface.h"
#include "InputHandler.h"
#include "CmdUi.h"
#include "QtUi.h"
#include "CameraControler.h"

AppManager::AppManager(char** argv) {
    std::cout << "App Manager created." << std::endl;
    is_running = true;
    Cameras = new std::vector<CameraControler*>();
    inputHandler_ = new InputHandler(this);

    if(std::string(argv[1]) == "cmd") {
        std::cout << "CMD selected" << std::endl;
        userInterface_ = new CmdUi(this);
    }
    else {
        std::cout << "QT selected" << std::endl;
        userInterface_ = new QtUi(this);
    }

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

bool AppManager::Start_App() {
    std::cout << "App Start ... " << std::endl;

    std::thread UI_Input_Thread([this]() { userInterface_->Update_Input(); });
    std::thread InputHandler_Thread([this]() { inputHandler_->Update(); });
    
    // TODO : utiliser un fichier de configuration pour ne pas avoir a reconfig a chaque fois
    Cameras->push_back(new CameraControler((this), "cam330", "169.254.1.222"));
    Cameras->push_back(new CameraControler((this), "cam310", "169.254.1.248"));
    
    // std::thread UI_Output_Thread([this]() { userInterface_->Update_Output(); });

    userInterface_->Update_Output();
    
    // UI_Output_Thread.join();
    UI_Input_Thread.join();
    InputHandler_Thread.join();
    for (CameraControler* cam : *Cameras)
    {
        cam->stop_Acquire();
    }

    return true;
}


std::vector<CameraControler*>* AppManager::Get_Cameras(){
    return this->Cameras; 
}



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