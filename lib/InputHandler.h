#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <string>
#include "ThreadSecureQueue.h"
#include "OutputPackage.h"

class AppManager;

class InputHandler {
public:
    InputHandler(AppManager* _appManager_);
    ~InputHandler();

    void Update();

private:
    AppManager* appManager_;
    ThreadSecureQueue<std::string*> *inputQueue = nullptr;
};

#endif