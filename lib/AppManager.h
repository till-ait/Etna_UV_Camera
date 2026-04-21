#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <atomic>

class UserInterface;
class InputHandler;
class CameraControler;
class SpectroControler;

class AppManager {
public :
    AppManager();
    AppManager(char** argv);
    ~AppManager();
    
    bool Start_App();

    std::vector<CameraControler*>* Get_Cameras();

    bool Get_Is_Running() const;
    void Set_Is_Running(bool value);

    UserInterface* Get_UserInterface();

    InputHandler* Get_InputHandler();

    SpectroControler* Get_Spectrometer();

private :
    std::vector<CameraControler*>* Cameras;
    std::atomic<bool> is_running;
    UserInterface* userInterface_;
    InputHandler* inputHandler_;
    SpectroControler* spectrometer;
};

#endif