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

class AppManager {
public :
    AppManager();
    ~AppManager();
    
    bool Start_App(void);   // TODO : creat interface and inputhandler threads

    std::vector<std::string> Get_id_Cameras();
    void Set_id_Cameras(const std::vector<std::string> _id_Cameras);
    
    std::vector<std::string> Get_Connected_Cameras();
    void Set_Connected_Cameras(const std::vector<std::string> _connected_Cameras);

    bool Get_Is_Running() const;
    void Set_Is_Running(bool value);

    UserInterface* Get_UserInterface();

private :
    std::vector<std::string> id_Cameras;       // TODO : switch type to PvString
    std::vector<std::string> connected_Cameras;  // TODO : switch type to *UvCamControler
    std::atomic<bool> is_running;
    UserInterface* userInterface_;
};

#endif