#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <vector>
#include <string>
#include "OutputPackage.h"
#include "ThreadSecureQueue.h"

// struct { // TODO : cette structur doit etre implementer dans camera et sont les information qui seront display 
//     std::string name;
//     bool is_connected;
//     bool is_streaming;
//     bool is_recording;
//     int fps;
// }camera_data;

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