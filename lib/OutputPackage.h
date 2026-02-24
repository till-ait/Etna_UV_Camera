#pragma once
#include <string>
#include <list>

class OutputPackage {
public :
    OutputPackage(std::string _source_name,
                  std::list<int>* _p_image_buffer,
                  int* _p_data_buffer);
    
    OutputPackage();

    std::string Get_Source_Name();
    std::list<int>* Get_P_Image_Buffer();
    int* Get_P_Data_Buffer();

private:
    std::string source_name;
    std::list<int> *p_image_buffer;    // TODO : remplacer par PVbuffer
    int *p_data_buffer;   // TODO : changer pour que ce soit 
};