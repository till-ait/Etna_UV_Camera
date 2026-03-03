#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
// #include "UserInterface.h"
// #include "CmdUi.h"

class UserInterface;
class InputHandler;
class CameraControler;

class AppManager {
public :
    AppManager();
    ~AppManager();

    // const std::string id_camera_330 = "169.254.1.222";
    // const std::string id_camera_310 = "169.254.1.248";
    
    bool Start_App(void);   // TODO : creat interface and inputhandler threads

    
    std::vector<CameraControler*>* Get_Cameras();
    // void Set_Cameras(const std::vector<std::string> _Cameras);

    bool Get_Is_Running() const;
    void Set_Is_Running(bool value);

    UserInterface* Get_UserInterface();

    InputHandler* Get_InputHandler();

private :
    // std::vector<s_camera_id> cameras_ids;       // TODO : switch type to PvString
    std::vector<CameraControler*>* Cameras;  // TODO : switch type to *UvCamControler
    std::atomic<bool> is_running;
    UserInterface* userInterface_;
    InputHandler* inputHandler_;
};

#endif