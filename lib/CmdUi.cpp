#include <iostream>
#include "CmdUi.h"
#include "AppManager.h"
#include "OutputPackage.h"
#include "InputHandler.h"

void CmdUi::Update_Output() {
    std::string msg;
    OutputPackage *current_output;
    while(appManager_->Get_Is_Running()) {
        current_output = outputQueue->pop();
        
        std::cout << "\n";

        // TODO : affiche l'image
        if(current_output->Get_P_Image_Buffer() != nullptr) {
            std::cout << "Display Image from " 
            << current_output->Get_Source_Name() 
            << std::endl;
        }

        // TODO : faire en sorte qu'il affiche la structur des parametre de la cam ou spectro
        if(current_output->Get_P_Data_Buffer() != nullptr) {
            std::cout << "Data : " 
            << *(current_output->Get_P_Data_Buffer()) 
            << " from" << current_output->Get_Source_Name()
            << std::endl;
        }

        if(current_output->Get_Display_Msg() != nullptr) {
            std::cout << *(current_output->Get_Display_Msg()) << std::endl;
        }
        std::cout << ">" << std::flush;

        delete current_output;
    }
    std::cout << "Output Thread closing ..." << std::endl;
}

void CmdUi::Update_Input() {
    std::string user_input;
    std::cout << "Input Thread running ..." << std::endl;

    while(appManager_->Get_Is_Running()) {
        std::getline(std::cin, user_input);

        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string(user_input));

    }
    std::cout << "Input Thread closing ..." << std::endl;
}