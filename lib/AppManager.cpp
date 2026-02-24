#include "AppManager.h"
#include "UserInterface.h"
#include "CmdUi.h"

AppManager::AppManager() {
    std::cout << "App Manager created." << std::endl;
    is_running = true;
    // userInterface_ = CmdUi(this);
    userInterface_ = std::make_unique<CmdUi>(this);
}

AppManager::~AppManager() {
    std::cout << "App Manager have been deleted." << std::endl;
}

bool AppManager::Start_App(void) {
    std::cout << "App Start ... " << std::endl;

    std::thread UI_Output_Thread([this]() { userInterface_->Update_Output(); });
    std::thread UI_Input_Thread([this]() { userInterface_->Update_Input(); });

    UI_Output_Thread.join();
    UI_Input_Thread.join();

    return true;
}

std::vector<std::string> AppManager::Get_id_Cameras(){
    return this->id_Cameras;
}

void AppManager::Set_id_Cameras(const std::vector<std::string> _id_Cameras) {
    this->id_Cameras = _id_Cameras;
}


std::vector<std::string> AppManager::Get_Connected_Cameras(){
    return this->connected_Cameras; 
}

void AppManager::Set_Connected_Cameras(const std::vector<std::string> _connected_Cameras) {
    this->connected_Cameras = _connected_Cameras;
}


bool AppManager::Get_Is_Running() const {
    return is_running;
}


void AppManager::Set_Is_Running(bool value){
    is_running = value;
}