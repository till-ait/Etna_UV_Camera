#include "OutputPackage.h"
#include "UserInterface.h"

OutputPackage::OutputPackage(AppManager *_appManager_, 
                std::string* _source_name,
                uint8_t* _p_image_buffer,
                uint32_t _width,
                uint32_t _height,
                int* _p_data_buffer,
                std::string* _display_msg)
: appManager_(_appManager_), source_name(_source_name), 
p_image_buffer(_p_image_buffer), width(_width), height(_height),
p_data_buffer(_p_data_buffer),display_msg(_display_msg) 
{
    appManager_->Get_UserInterface()->GetOutputQueue()->push(this);
}

// TODO : delet all the cout et iostream and use outputpackage
OutputPackage::OutputPackage(AppManager *_appManager_, std::string* _display_msg)
: OutputPackage(_appManager_, new std::string(""), nullptr, 0, 0, nullptr, _display_msg)
{}

OutputPackage::OutputPackage(AppManager *_appManager_, std::string* _source_name, uint8_t* _p_image_buffer, uint32_t _width, uint32_t _height)
: OutputPackage(_appManager_, _source_name, _p_image_buffer, _width, _height, nullptr, nullptr)
{}

OutputPackage::OutputPackage(AppManager *_appManager_, std::string* _source_name, int* _p_data_buffer)
: OutputPackage(_appManager_, _source_name, nullptr, 0, 0, _p_data_buffer, nullptr)
{}


OutputPackage::OutputPackage() 
: appManager_(nullptr), source_name(new std::string("")), p_image_buffer(nullptr), width(0), height(0), p_data_buffer(nullptr), display_msg(nullptr) 
{}

OutputPackage::~OutputPackage() {
    delete source_name;
    delete p_image_buffer;    // TODO : remplacer par PVbuffer
    delete p_data_buffer;   // TODO : changer pour que ce soit 
    delete display_msg;
}

std::string* OutputPackage::Get_Source_Name(){
    return source_name;
}

uint8_t* OutputPackage::Get_P_Image_Buffer(){
    return p_image_buffer;
}

uint32_t OutputPackage::Get_Width() {
    return width;
}

uint32_t OutputPackage::Get_Height() {
    return height;
}

int* OutputPackage::Get_P_Data_Buffer(){
    return p_data_buffer;
}

std::string* OutputPackage::Get_Display_Msg() {
    return display_msg;
}