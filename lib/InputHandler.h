#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <string>
#include <sstream>
#include <vector>
#include <semaphore>
#include "ThreadSecureQueue.h"
#include "OutputPackage.h"

class AppManager;

class InputHandler {
public:
    InputHandler(AppManager* _appManager_);
    ~InputHandler();

    void Update();

    ThreadSecureQueue<std::string*>* Get_InputQueue();
    std::binary_semaphore& Get_Input_Handled();

private:
    AppManager* appManager_;
    ThreadSecureQueue<std::string*> *inputQueue = nullptr;
    std::binary_semaphore signal_Input_Handled{0};

    std::vector<std::string> split(const std::string& str);

    void exit_cmd();
    void set_cmd(const std::vector<std::string>& split_input);
    void default_cmd(const std::vector<std::string>& split_input);
};

#endif