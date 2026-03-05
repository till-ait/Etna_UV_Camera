
#include <semaphore>
#include "QtUI.h"
#include "InputHandler.h"

void QtUi::Update_Output() {
    std::cout << "Output Thread running ..." << std::flush;

    

    std::cout << "Output Thread closing ..." << std::endl;
}

void QtUi::Update_Input() {
    std::string user_input;
    
    std::binary_semaphore& signal_Input_Handled = appManager_->Get_InputHandler()->Get_Input_Handled();

    while(appManager_->Get_Is_Running()) {
        // std::getline(std::cin, user_input);
        user_input = "exit";

        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string(user_input));

        signal_Input_Handled.acquire();
    }
    std::cout << "Input Thread closing ..." << std::endl;
}