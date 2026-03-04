#include "InputHandler.h"
#include "CameraControler.h"
#include "AppManager.h"

#define INDEX_CMD 0
#define INDEX_DEVICE_NAME 1
#define INDEX_ARG_NAME 2
#define INDEX_ARG_VALUE 3


InputHandler::InputHandler(AppManager* _appManager_) 
: appManager_(_appManager_), inputQueue(new ThreadSecureQueue<std::string*>())
{}

InputHandler::~InputHandler() {
    delete inputQueue;
}

void InputHandler::Update() {
    std::string *input;

    while(appManager_->Get_Is_Running()) {
        input = inputQueue->pop();

        std::vector<std::string> split_input = split(*input);

        if(split_input[INDEX_CMD] == "exit") {
            exit_cmd();
        }
        else if(split_input[INDEX_CMD] == "connect") {
            connect_cmd(split_input);
        }
        else if(split_input[INDEX_CMD] == "set") {
            set_cmd(split_input);
        }
        else if(split_input[INDEX_CMD] == "help") {
            help_cmd();
        }
        else if(split_input[INDEX_CMD] == "unblock_getline") {
            // do nothing
        }
        else {
            default_cmd(split_input);
        }

        signal_Input_Handled.release();
    }

    delete input;
}

std::vector<std::string> InputHandler::split(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}

void InputHandler::exit_cmd(){
    new OutputPackage(appManager_, new std::string("Terminate Threads... "));
    appManager_->Set_Is_Running(false);
}

void InputHandler::connect_cmd(const std::vector<std::string>& split_input){
    bool result = false;
    std::string camera_id;
    
    std::vector<CameraControler*>* cameras = appManager_->Get_Cameras();

    for(int i=0; i<cameras->size(); i++) {
        if(cameras->at(i)->Get_Data().name == split_input[1]) {
            result = cameras->at(i)->Try_Connection();

            if(!result) {
                new OutputPackage(appManager_, new std::string("Connection failed."));
                return;
            }

            cameras->at(i)->start_Acquire();
        }
    }

    if(!result) {
        new OutputPackage(appManager_, new std::string("Device not found."));
        return;
    }


}

void InputHandler::set_cmd(const std::vector<std::string>& split_input){
    CameraControler* selected_camera = NULL;

    for (CameraControler* cam : *(appManager_->Get_Cameras())){
        if(cam->Get_Data().name == split_input[INDEX_DEVICE_NAME]){
            selected_camera = cam;
        }
    }

    if(selected_camera == NULL) {
        new OutputPackage(appManager_, new std::string("Device not found."));
        return;
    }

    if(split_input[INDEX_ARG_NAME] == "fps") {
        try {
            selected_camera->Set_Fps(std::stoi(split_input[INDEX_ARG_VALUE]));
        } catch (const std::invalid_argument&) {
            new OutputPackage(appManager_, new std::string("Err : value must be an int."));
        }
    }
}

void InputHandler::help_cmd() {
    std::string msg = "Commande list : \n";

    msg += " - connect {cam330 or cam310} : try to connect to the designeted camera.\n";
    msg += " - exit : exit programme.\n";
    msg += " - set {target} {args} {value} : set target's argument value. (args : fps/width/eight)\n";
    msg += " - help : print commands.\n";

    new OutputPackage(appManager_, new std::string(msg));
}

void InputHandler::default_cmd(const std::vector<std::string>& split_input) {
    std::string msg = "Err : '";
    msg += split_input[0];
    msg += "' isn't a existing commmand, enter 'help' to see the available commads.";

    new OutputPackage(appManager_, new std::string(msg));
}

ThreadSecureQueue<std::string*>* InputHandler::Get_InputQueue() {
    return inputQueue;
}

std::binary_semaphore& InputHandler::Get_Input_Handled() {
    return signal_Input_Handled;
}