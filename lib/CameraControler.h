#ifndef CAMERACONTROLER_H
#define CAMERACONTROLER_H

#include <process.h>
#include <conio.h>

#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
#include <thread>
#include <mutex>


#define DEFAULT_FPS 16
#define BUFFER_COUNT ( 16 )

#define MIN_EXPOSURE_TIME 118
#define MAX_EXPOSURE_TIME 61967
#define DEFAULT_EXPOSURE_TIME 31000

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
    int exposure_time;
    float gain;
};

class CameraControler {
public :
    CameraControler(AppManager* _appManager_, 
                    std::string name,
                    std::string id);
    CameraControler(AppManager* _appManager_);
    ~CameraControler();

    s_camera_data Get_Data();
    void Set_Is_Connected(bool new_state);
    void Set_Is_Streaming(bool new_state);
    void Set_Is_Recording(bool new_state);

    bool Try_Connection();
    
    void start_Acquire();
    void stop_Acquire();

    int Get_Fps();
    void Set_Fps(int _fps);

    float getGain();
    void setGain(float value);

    void Set_Exposure_Time(int value);

private :
    AppManager* appManager_;
    s_camera_data data;
    PvDevice *device;
    PvStream *stream;
    PvResult *pv_result;
    PvDeviceGEV* device_GEV;
    PvStreamGEV* stream_GEV;
    std::list<PvBuffer *>* buffer_list;
    std::thread* thread_acquire;
    std::mutex data_gain_mutex;

    void Create_Stream_Buffers();
    void Free_Stream_Buffers();
    void Acquire_Images();
    void Send_Fps();
    void Print_Param();
};

#endif