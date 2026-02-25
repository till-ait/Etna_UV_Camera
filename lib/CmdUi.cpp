#include <iostream>
#include "CmdUi.h"
#include "AppManager.h"
#include "OutputPackage.h"

void CmdUi::Update_Output() {
    std::string msg;
    OutputPackage *current_output;
    while(appManager_->Get_Is_Running()) {
        msg = "\rOutput Thread running ... Dislplay cameras datas";
        std::cout << msg << std::endl;
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

        delete current_output;
    }
    std::cout << "Input Thread closing ..." << std::endl;
}

void CmdUi::Update_Input() {
    std::string user_input;
    std::cout << "Input Thread running ..." << std::endl;

    while(appManager_->Get_Is_Running()) {
        std::cout << ">" << std::flush;
        // std::cin.clear();
        // user_input = std::cin.get();
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, user_input);

        new OutputPackage(appManager_, "input", nullptr, new int(6), new std::string("My message"));
        
        // outputQueue->push(generated_output);

        if(user_input == "stop") {
            appManager_->Set_Is_Running(false);
            outputQueue->stop();
        }   // TODO : cree l'input package avec la commande et tout ici et l'envoyer dans la queu du input hendler
    }
    std::cout << "Input Thread closing ..." << std::endl;
}