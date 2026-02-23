#pragma once 
#include "UserInterface.h"

class CmdUi : public UserInterface {
public:
    CmdUi(AppManager* appManager_) : UserInterface(appManager_)
    {}

    void Update() override;
};