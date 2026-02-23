#include "AppManager.h"

AppManager::AppManager() {
    std::cout << "App Manager created." << std::endl;
}

AppManager::~AppManager() {
    std::cout << "App Manager have been deleted." << std::endl;
}

bool AppManager::Start_App(void) {
    std::cout << "App Start ... " << std::endl;
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
