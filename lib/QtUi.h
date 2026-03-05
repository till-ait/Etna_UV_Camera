#ifndef QTUI_H
#define QTUI_H

#include "UserInterface.h"
#include <string>

class QtUi : public UserInterface {
public:
    QtUi(AppManager* appManager_) : UserInterface(appManager_)
    {}

    void Update_Output() override;
    void Update_Input() override;
};

#endif