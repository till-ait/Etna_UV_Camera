#include <string>
#include <list>
#include "UserInterface.h"

class OutputPackage {
public :
    OutputPackage(const string source_name,
                  const list<int>* _p_image_buffer,
                  const int* _p_data_buffer);

    string Get_Source_Name();
    list<int>* Get_P_Image_Buffer();
    int* Get_P_Data_Buffer();

private:
    const string source_name;
    const list<int> *p_image_buffer;    // TODO : remplacer par PVbuffer
    const int *p_data_buffer;   // TODO : changer pour que ce soit 
}