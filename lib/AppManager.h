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
    
    bool Start_App(void);

    std::vector<CameraControler*>* Get_Cameras();

    bool Get_Is_Running() const;
    void Set_Is_Running(bool value);

    UserInterface* Get_UserInterface();

    InputHandler* Get_InputHandler();

private :
    std::vector<CameraControler*>* Cameras;
    std::atomic<bool> is_running;
    UserInterface* userInterface_;
    InputHandler* inputHandler_;
};

#endif