#include <iostream>
#include <cstdio>
#include <semaphore>
#include "CmdUi.h"
#include "AppManager.h"
#include "OutputPackage.h"
#include "InputHandler.h"
#include "UVCamDisplay.h"
#include "CameraControler.h"

void CmdUi::Update_Output() {
    std::string msg;
    OutputPackage *current_output;

    std::cout << "Output Thread running ..." << std::flush;

    // TODO : faire en sorte qu'il cree autant de fenetre que de cam et verif avec GetTitle
    // UVCamDisplay cam330("Cam330");
    // cam330.Open(1280, 1024);
    // UVCamDisplay cam310("Cam310");
    // cam310.Open(1280, 1024);

    std::vector<UVCamDisplay*> cam_windows;

    for (CameraControler* cam : *(appManager_->Get_Cameras()))
    {
        cam_windows.push_back(new UVCamDisplay(cam->Get_Data().name));
        cam_windows.back()->Open(500, 500);
    }

    while(appManager_->Get_Is_Running()) {
        current_output = outputQueue->pop();
        
        // std::cout << "\n";

        // TODO : affiche l'image
        if(current_output->Get_P_Image_Buffer() != nullptr) {

            uint8_t *data   = current_output->Get_P_Image_Buffer();
            uint32_t width  = current_output->Get_Width();
            uint32_t height = current_output->Get_Height();

            
            // if(*(current_output->Get_Source_Name()) == "cam330"){
            //     cam330.PushFrame(data, width, height);
            // }
            
            // if(*(current_output->Get_Source_Name()) == "cam310"){
            //     cam310.PushFrame(data, width, height);
            // }
            
            for (UVCamDisplay* window : cam_windows)
            {
                if(*(current_output->Get_Source_Name()) == window->GetTitle()){
                    window->PushFrame(data, width, height);
                }
            }
        }

        // TODO : faire en sorte qu'il affiche la structur des parametre de la cam ou spectro
        if(current_output->Get_P_Data_Buffer() != nullptr) {
            std::cout << "Data : " 
            << *(current_output->Get_P_Data_Buffer()) 
            << " from" << *(current_output->Get_Source_Name())
            << std::endl;
            std::cout << ">" << std::flush;
        }

        if(current_output->Get_Display_Msg() != nullptr) {
            std::cout << *(current_output->Get_Display_Msg()) << std::endl;
            std::cout << ">" << std::flush;
        }
        
        if(appManager_->Get_Is_Running()){
            delete current_output;
        }
    }

    for (UVCamDisplay* window : cam_windows)
    {
        window->Close();
        delete window;
    }

    std::cout << "Output Thread closing ..." << std::endl;
}

void CmdUi::Update_Input() {
    std::string user_input;
    //std::cout << "Input Thread running ..." << std::endl;
    new OutputPackage(appManager_, new std::string("Input Thread running ..."));
    std::binary_semaphore& signal_Input_Handled = appManager_->Get_InputHandler()->Get_Input_Handled();

    while(appManager_->Get_Is_Running()) {
        std::getline(std::cin, user_input);

        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string(user_input));

        signal_Input_Handled.acquire();
    }
    std::cout << "Input Thread closing ..." << std::endl;
}