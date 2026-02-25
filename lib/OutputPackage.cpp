#include "OutputPackage.h"
#include "UserInterface.h"

OutputPackage::OutputPackage(AppManager *_appManager_, 
                std::string _source_name,
                std::list<int>* _p_image_buffer,
                int* _p_data_buffer,
                std::string* _display_msg)
: appManager_(_appManager_), source_name(_source_name), p_image_buffer(_p_image_buffer), p_data_buffer(_p_data_buffer),display_msg(_display_msg) 
{
    appManager_->Get_UserInterface()->GetOutputQueue()->push(this);
}

OutputPackage::OutputPackage() 
: appManager_(nullptr), source_name(""), p_image_buffer(nullptr), p_data_buffer(nullptr), display_msg(nullptr) {}

OutputPackage::~OutputPackage() {
    delete p_image_buffer;    // TODO : remplacer par PVbuffer
    delete p_data_buffer;   // TODO : changer pour que ce soit 
    delete display_msg;
}

std::string OutputPackage::Get_Source_Name(){
    return source_name;
}

std::list<int>* OutputPackage::Get_P_Image_Buffer(){
    return p_image_buffer;
}

int* OutputPackage::Get_P_Data_Buffer(){
    return p_data_buffer;
}

std::string* OutputPackage::Get_Display_Msg() {
    return display_msg;
}