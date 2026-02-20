#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <iostream>
#include <vector>
#include <string>

class AppManager {
public :
    AppManager();
    ~AppManager();
    
    bool Start_App(void);   // TODO : creat interface and inputhandler threads

    std::vector<std::string> Get_id_Cameras();
    void Set_id_Cameras(const std::vector<std::string> _id_Cameras);
    
    std::vector<std::string> Get_Connected_Cameras();
    void Set_Connected_Cameras(const std::vector<std::string> _connected_Cameras);

private :
    std::vector<std::string> id_Cameras;       // TODO : switch type to PvString
    std::vector<std::string> connected_Cameras;  // TODO : switch type to *UvCamControler
};

#endif