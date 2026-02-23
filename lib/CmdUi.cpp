#include <iostream>
#include "CmdUi.h"
#include "AppManager.h"

void CmdUi::Update() {
    while(appManager_->Get_Is_Running()) {
        std::cout << "BOUCLE UPDATE UI" << std::endl;
        appManager_->Set_Is_Running(false);
    }
}