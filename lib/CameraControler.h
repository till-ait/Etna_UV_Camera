#ifndef CAMERACONTROLER_H
#define CAMERACONTROLER_H

#include <process.h>
#include <conio.h>

#include <stdio.h>
#include <string>
// #include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>


#define DEFAULT_FPS 16
#define BUFFER_COUNT ( 16 )

class AppManager;
class OutputPackage;
class PvDevice;
class PvStream;
class PvResult;
class PvDeviceGEV;
class PvStreamGEV;
class PvBuffer;

struct s_camera_data {
    std::string name;
    std::string id;
    bool is_connected;
    bool is_streaming;
    bool is_recording;
    int fps;
};

class CameraControler {
public :
    CameraControler(AppManager* _appManager_, 
                    std::string name,
                    std::string id);
    CameraControler(AppManager* _appManager_);

    s_camera_data Get_Data();
    void Set_Is_Connected(bool new_state);
    void Set_Is_Streaming(bool new_state);
    void Set_Is_Recording(bool new_state);

    bool Try_Connection();
    void Acquire_Images();
    
private :
    AppManager* appManager_;
    s_camera_data data;
    PvDevice *device;
    PvStream *stream;
    PvResult *pv_result;
    PvDeviceGEV* device_GEV;
    PvStreamGEV* stream_GEV;
    std::list<PvBuffer *>* buffer_list;

    void Create_Stream_Buffers();
    void Free_Stream_Buffers();
};

#endif