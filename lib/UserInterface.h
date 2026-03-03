#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <vector>
#include <string>
#include "OutputPackage.h"
#include "ThreadSecureQueue.h"

class AppManager;

class UserInterface {
public :
    UserInterface(AppManager *_appManager_);
    virtual ~UserInterface();

    virtual void Update_Output() =0;
    virtual void Update_Input() =0;

    ThreadSecureQueue<OutputPackage*>* GetOutputQueue(void);

protected :
    AppManager *appManager_;
    ThreadSecureQueue<OutputPackage*> *outputQueue = nullptr;
};

#endif