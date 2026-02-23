#include "UserInterface.h"

UserInterface::UserInterface(AppManager* _appManager_) : appManager_(_appManager_)
{}

ThreadSecureQueue<OutputPackage>* UserInterface::GetOutputQueue(void) {
    return outputQueue;
}