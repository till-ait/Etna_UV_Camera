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
    virtual void Push_Frame(std::string* source_Name, uint8_t *data, uint32_t width, uint32_t height) =0;

    ThreadSecureQueue<OutputPackage*>* GetOutputQueue(void);

protected :
    AppManager *appManager_;
    ThreadSecureQueue<OutputPackage*> *outputQueue = nullptr;
};

#endif