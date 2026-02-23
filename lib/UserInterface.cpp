#include "UserInterface"

UserInterface::UserInterface(const AppManager* _appManager_) : appManager_(_appManager_)
{}

UserInterface::ThreadSecureQueue* GetOutputQueue(void) {
    return outputQueue;
}