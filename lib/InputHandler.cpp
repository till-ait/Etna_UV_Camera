#include "InputHandler.h"


InputHandler::InputHandler(AppManager* _appManager_) 
: appManager_(_appManager_), inputQueue(new ThreadSecureQueue<std::string*>())
{}

InputHandler::~InputHandler() {
    delete inputQueue;
}

void InputHandler::Update() {
    new OutputPackage(appManager_, new std::string("InputHandler Update throught UI !"));

    std::string *input;

    while(appManager_->Get_Is_Running()) {
        input = inputQueue->pop();

        new OutputPackage(appManager_, input);

        std::vector<std::string> split_input = split(*input);

        if(split_input[0] == "exit") {
            exit_cmd();
        }
        else if(split_input[0] == "set") {
            set_cmd(split_input);
        }
        else if(split_input[0] == "unblock_getline") {
            // do nothing
        }
        else {
            default_cmd(split_input);
        }

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
    new OutputPackage(appManager_, new std::string("try shut down"));
    appManager_->Set_Is_Running(false);
    // TODO : Le exit ici ne fait pas quitter le getLine de l'autre thread
    // peut etre utiliser un mutex pour l'empecher d'aller dans le getLine
}

void InputHandler::set_cmd(const std::vector<std::string>& split_input){
    // TODO : Implement la logique
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