#pragma once
#include <string>
#include <list>
#include "AppManager.h"

class OutputPackage {
public :
    OutputPackage(AppManager *_appManager_,
                  std::string _source_name,
                  std::list<int>* _p_image_buffer,
                  int* _p_data_buffer,
                  std::string* _display_msg);
    
    OutputPackage();
    ~OutputPackage();

    std::string Get_Source_Name();
    std::list<int>* Get_P_Image_Buffer();
    int* Get_P_Data_Buffer();
    std::string* Get_Display_Msg();

private:
    AppManager *appManager_;
    std::string source_name;
    std::list<int> *p_image_buffer;    // TODO : remplacer par PVbuffer
    int *p_data_buffer;   // TODO : changer pour que ce soit 
    std::string* display_msg;
};