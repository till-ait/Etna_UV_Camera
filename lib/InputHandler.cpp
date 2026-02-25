#include "InputHandler.h"


InputHandler::InputHandler(AppManager* _appManager_) 
: appManager_(_appManager_), inputQueue(new ThreadSecureQueue<std::string*>())
{}

InputHandler::~InputHandler() {
    delete inputQueue;
}

void InputHandler::Update() {
    new OutputPackage(appManager_, new std::string("InputHandler Update throught UI !"));
}