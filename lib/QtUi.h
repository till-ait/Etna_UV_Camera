#ifndef QTUI_H
#define QTUI_H

#include "UserInterface.h"
#include <string>
#include <vector>
#include "MainWindow.h"

class QtUi : public UserInterface{
public:
    QtUi(AppManager* appManager_) : UserInterface(appManager_)
    {}

    void Update_Output() override;
    void Update_Output2();
    void Update_Input() override;
    void Push_Frame(std::string* source_Name, uint8_t *data, uint32_t width, uint32_t height) override;
    void Push_Spectrum(std::vector <double> spectrum, std::vector <double> wavelengths) override;
    void Ui_Print(std::string msg) override;

    MainWindow* window = nullptr;
};

#endif