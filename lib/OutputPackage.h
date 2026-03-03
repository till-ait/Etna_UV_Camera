#ifndef OUTPUTPACKAGE_H
#define OUTPUTPACKAGE_H

#include <string>
#include <list>
#include "AppManager.h"

class OutputPackage {
public :
    OutputPackage(AppManager *_appManager_,
                  std::string* _source_name,
                  uint8_t* _p_image_buffer,
                  uint32_t _width,
                  uint32_t _height,
                  int* _p_data_buffer,
                  std::string* _display_msg);

    OutputPackage(AppManager *_appManager_, std::string* _display_msg);
    OutputPackage(AppManager *_appManager_, std::string* _source_name, uint8_t* _p_image_buffer, uint32_t _width, uint32_t _height);
    OutputPackage(AppManager *_appManager_, std::string* _source_name, int* _p_data_buffer);
    
    OutputPackage();
    ~OutputPackage();

    std::string* Get_Source_Name();
    uint8_t* Get_P_Image_Buffer();
    uint32_t Get_Width();
    uint32_t Get_Height();
    int* Get_P_Data_Buffer();
    std::string* Get_Display_Msg();

private:
    AppManager *appManager_;
    std::string* source_name;
    uint8_t* p_image_buffer;
    uint32_t width;
    uint32_t height;
    int *p_data_buffer;   // TODO : changer pour que ce soit 
    std::string* display_msg;
};

#endif