#ifndef SPECTROCONTROLER_H
#define SPECTROCONTROLER_H

#define NOMINMAX

#include <iostream>
#include <windows.h>
#include <winusb.h>
#include <setupapi.h>
#include <initguid.h>
#include <usbiodef.h>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <sstream>

#define EP_OUT 0x01
#define EP_IN_DATA_1 0x82
#define EP_IN_DATA_2 0x86
#define EP_IN_CMD  0x81

#define CMD_INIT             0x01
#define CMD_SET_INTEG_TIME   0x02
#define CMD_REQUEST_SPECTRUM 0x09
#define CMD_QUERY_EEPROM     0x05
#define CMD_SET_SCANS_AVG    0x49
#define CMD_QUERY_INFO       0xfe
#define CMD_TRIGGER_MODE     0x0A

#define TIMEOUT_MS      5000
#define SPECTRUM_PIXELS 2048

#define CMD_BUF_LENGTH 64
#define DATA_BUF_LENGTH 512

#define DEFAULT_INTEGRATION_TIME 5000  // micro sec
#define MAX_INTEGRATION_TIME 60000000
#define MIN_INTEGRATION_TIME 1
#define DEFAULT_AVERAGING 5
#define DEFAULT_TRIGGER_MODE 0
#define SPECTRUM_LENGTH 3840
#define DEFAULT_AVERAGING_ALPHA 0.1

class AppManager;

class SpectroControler {
public :
    SpectroControler(AppManager *_appManager_);
    ~SpectroControler();

    bool Connect();
    bool Is_Connected();
    bool Set_integration_time(uint32_t micros);
    int Get_integration_time();
    bool Set_scans_to_average(uint16_t scans);
    int Get_scans_to_average();
    std::string Get_serial_number();
    bool Set_trigger_mode(uint8_t mode);
    void Get_spectrum();
    bool start_Acquire();
    void stop_Acquire();
    void LoadCalibration();
    double Get_calibration_coef(int index);
    double smoothingToAlpha(int level);
    std::string ReadEEPROM(uint8_t slot);
    double PixelToWavelength(int p);
    void ProcessSpectrum(const std::vector<double>& raw, std::vector<double>& wavelengths, std::vector<double>& intensities);

private :
    AppManager* appManager_;
    bool is_connected;
    bool is_streaming;
    const std::string TARGET_VID = "vid_2457";
    const std::string TARGET_PID = "pid_1022";

    GUID WINUSB_GUID = 
    {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

    HDEVINFO deviceInfo;
    HANDLE deviceHandle;
    SP_DEVICE_INTERFACE_DATA ifaceData;
    WINUSB_INTERFACE_HANDLE usbHandle;
    bool usbHandleInitialized;
    UCHAR shortPacket;
    ULONG timeout;
    ULONG transferred;
    BOOL trasfert_result;

    uint8_t cmdBufOut[CMD_BUF_LENGTH] = {};
    uint8_t cmdBufIn[CMD_BUF_LENGTH] = {};
    uint8_t dataBuf[DATA_BUF_LENGTH] = {};

    uint32_t integration_time;
    uint16_t scans_average;

    double wl_coef[4];
    std::vector<double> average_spectrum = std::vector<double>(SPECTRUM_LENGTH, 0.0);
    float alpha_coef = DEFAULT_AVERAGING_ALPHA;

    std::thread* thread_acquire;
    std::string serial_number;
};

#endif