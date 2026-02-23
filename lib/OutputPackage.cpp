#include "OutputPackage.h"

OutputPackage::string Get_Source_Name(){
    return source_name;
}

OutputPackage::list<int>* Get_P_Image_Buffer(){
    return p_image_buffer;
}

OutputPackage::int* Get_P_Data_Buffer(){
    return p_data_buffer;
}