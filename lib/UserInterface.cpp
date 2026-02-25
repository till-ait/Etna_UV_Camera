#include "UserInterface.h"

UserInterface::UserInterface(AppManager* _appManager_) : appManager_(_appManager_),
                            outputQueue(new ThreadSecureQueue<OutputPackage*>())
{}

ThreadSecureQueue<OutputPackage*>* UserInterface::GetOutputQueue(void) {
    return outputQueue;
}