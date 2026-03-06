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

    std::vector<UVCamDisplay*> cam_windows;

    for (CameraControler* cam : *(appManager_->Get_Cameras()))
    {
        cam_windows.push_back(new UVCamDisplay(cam->Get_Data().name));
        cam_windows.back()->Open(500, 500);
    }

    while(appManager_->Get_Is_Running()) {
        current_output = outputQueue->pop();

        // If too many image in the queue skip
        if((current_output->Get_P_Image_Buffer() != nullptr)
            && (outputQueue->size() < 10)) {

            uint8_t *data   = current_output->Get_P_Image_Buffer();
            uint32_t width  = current_output->Get_Width();
            uint32_t height = current_output->Get_Height();
            
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
    
    new OutputPackage(appManager_, new std::string("Input Thread running ..."));
    std::binary_semaphore& signal_Input_Handled = appManager_->Get_InputHandler()->Get_Input_Handled();

    while(appManager_->Get_Is_Running()) {
        std::getline(std::cin, user_input);

        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string(user_input));

        signal_Input_Handled.acquire();
    }
    std::cout << "Input Thread closing ..." << std::endl;
}

void CmdUi::Push_Frame(std::string* source_Name, uint8_t *data, uint32_t width, uint32_t height) {
    new OutputPackage(appManager_, source_Name, data, width, height);
}