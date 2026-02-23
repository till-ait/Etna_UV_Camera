#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <vector>
#include <string>
#include "AppManager.h"
#include "OutputPayload.h"
#include "ThreadSecureQueue.h"

// struct { // TODO : cette structur doit etre implementer dans camera et sont les information qui seront display 
//     std::string name;
//     bool is_connected;
//     bool is_streaming;
//     bool is_recording;
//     int fps;
// }camera_data;



// TODO il faut que je fass la class ThreadSecureQueue; OutputPayload; InputPayload; 

enum e_output_type {
    IMAGE,
    DISPLAYED_INFO
};

struct {
    e_output_type output_type;
    OutputPayload output_payload;
}output_pakage;

class UserInterface {
public :
    UserInterface(const AppManager *_appManager_);
    virtual ~UserInterface() =default;

    virtual void Update() =0;

    ThreadSecureQueue* GetOutputQueue(void);

protected :
    const AppManager *appManager_;
    const ThreadSecureQueue *outputQueue<output_pakage>
};

#endif