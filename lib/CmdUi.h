#pragma once 
#include "UserInterface.h"
#include <string>

class CmdUi : public UserInterface {
public:
    CmdUi(AppManager* appManager_) : UserInterface(appManager_)
    {}

    void Update_Output() override;
    void Update_Input() override;
};