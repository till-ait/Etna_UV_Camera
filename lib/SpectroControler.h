#ifndef SPECTROCONTROLER_H
#define SPECTROCONTROLER_H

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

#define EP_OUT 0x01
#define EP_IN_DATA_1 0x02
#define EP_IN_DATA_2 0x06
#define EP_IN_CMD  0x81

#define CMD_INIT             0x01
#define CMD_SET_INTEG_TIME   0x02
#define CMD_REQUEST_SPECTRUM 0x09
#define CMD_QUERY_EEPROM     0x05
#define CMD_SET_SCANS_AVG    0x49
#define CMD_QUERY_INFO       0xfe

#define TIMEOUT_MS      5000

#define CMD_BUF_LENGTH 64
#define DATA_BUF_LENGTH 512

class AppManager;

class SpectroControler {
public :
    SpectroControler(AppManager *_appManager_);
    ~SpectroControler();

    bool Connect();
    bool Is_Connected();

private :
    AppManager* appManager_;
    bool is_connected;
    const std::string TARGET_VID = "vid_2457";
    const std::string TARGET_PID = "pid_1022";

    GUID WINUSB_GUID = 
    {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

    HDEVINFO deviceInfo;
    HANDLE deviceHandle;
    SP_DEVICE_INTERFACE_DATA ifaceData;
    WINUSB_INTERFACE_HANDLE usbHandle;
    UCHAR shortPacket;
    ULONG timeout;
    ULONG transferred;
    BOOL trasfert_result;

    uint8_t cmdBufOut[CMD_BUF_LENGTH] = {};
    uint8_t cmdBufIn[CMD_BUF_LENGTH] = {};
    uint8_t dataBuf[DATA_BUF_LENGTH] = {};
};

#endif