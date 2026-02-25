#include "OutputPackage.h"

OutputPackage::OutputPackage(std::string _source_name,
                std::list<int>* _p_image_buffer,
                int* _p_data_buffer,
                std::string _display_msg)
: source_name(_source_name), p_image_buffer(_p_image_buffer), p_data_buffer(_p_data_buffer),display_msg(_display_msg) {}

OutputPackage::OutputPackage() 
: source_name(""), p_image_buffer(nullptr), p_data_buffer(nullptr), display_msg("") {}

std::string OutputPackage::Get_Source_Name(){
    return source_name;
}

std::list<int>* OutputPackage::Get_P_Image_Buffer(){
    return p_image_buffer;
}

int* OutputPackage::Get_P_Data_Buffer(){
    return p_data_buffer;
}

std::string OutputPackage::Get_Display_Msg() {
    return display_msg;
}