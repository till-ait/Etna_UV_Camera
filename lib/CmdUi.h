#ifndef CMDUI_H
#define CMDUI_H

#include "UserInterface.h"
#include <string>

class CmdUi : public UserInterface {
public:
    CmdUi(AppManager* appManager_) : UserInterface(appManager_)
    {}

    void Update_Output() override;
    void Update_Input() override;
    void Push_Frame(std::string* source_Name, uint8_t *data, uint32_t width, uint32_t height) override;
};

#endif