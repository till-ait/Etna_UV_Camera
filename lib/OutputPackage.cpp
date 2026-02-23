#include "OutputPackage.h"

std::string OutputPackage::Get_Source_Name(){
    return source_name;
}

std::list<int>* OutputPackage::Get_P_Image_Buffer(){
    return p_image_buffer;
}

int* OutputPackage::Get_P_Data_Buffer(){
    return p_data_buffer;
}