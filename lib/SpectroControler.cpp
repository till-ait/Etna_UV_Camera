#include "SpectroControler.h"
#include "AppManager.h"
#include "UserInterface.h"



SpectroControler::SpectroControler(AppManager *_appManager_) {
    std::cout << "Spectro cree" << std::endl;
    appManager_ = _appManager_;
    is_connected = false;
    shortPacket = 0;
    timeout = 2000;
    transferred = 0;
    trasfert_result = FALSE;
    memset(cmdBufOut, 0, CMD_BUF_LENGTH);
    memset(cmdBufIn, 0, CMD_BUF_LENGTH);
    memset(dataBuf, 0, DATA_BUF_LENGTH);
}

SpectroControler::~SpectroControler() {
    std::cout << "Spectro detruit" << std::endl;
}

bool SpectroControler::Is_Connected() {
    return is_connected;
}

bool SpectroControler::Connect() {
    is_connected = false;

    deviceInfo = SetupDiGetClassDevs(
        &WINUSB_GUID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    HANDLE deviceHandle = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA ifaceData = {};
    ifaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD index = 0; ; index++) {
        if (!SetupDiEnumDeviceInterfaces(deviceInfo, NULL, &WINUSB_GUID, index, &ifaceData)) {
            appManager_->Get_UserInterface()->Ui_Print("Err : No USB2000+ detected");
            SetupDiDestroyDeviceInfoList(deviceInfo);
            return false;
        }
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfo, &ifaceData, NULL, 0, &requiredSize, NULL);
        auto detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SetupDiGetDeviceInterfaceDetail(deviceInfo, &ifaceData, detailData, requiredSize, NULL, NULL);
        std::string path = detailData->DevicePath;
        if (path.find(TARGET_VID) != std::string::npos &&
            path.find(TARGET_PID) != std::string::npos) {
            deviceHandle = CreateFile(detailData->DevicePath,
                GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
            free(detailData);
            break;
        }
        free(detailData);
    }
    SetupDiDestroyDeviceInfoList(deviceInfo);

    if (!WinUsb_Initialize(deviceHandle, &usbHandle)) {
        appManager_->Get_UserInterface()->Ui_Print("Err : WinUsb_Initialize failed.");
        return false;
    }

    shortPacket = 0;
    WinUsb_SetPipePolicy(usbHandle, 0x01, SHORT_PACKET_TERMINATE, sizeof(shortPacket), &shortPacket);
    WinUsb_SetPipePolicy(usbHandle, 0x02, SHORT_PACKET_TERMINATE, sizeof(shortPacket), &shortPacket);

    timeout = 2000;
    for (UCHAR ep : {EP_OUT, EP_IN_DATA_1, EP_IN_DATA_2, EP_IN_CMD}) {
        WinUsb_SetPipePolicy(usbHandle, ep, PIPE_TRANSFER_TIMEOUT, sizeof(timeout), &timeout);
    }

    transferred = 0;
    trasfert_result = FALSE;
    cmdBufOut[0] = CMD_INIT;
    trasfert_result = WinUsb_WritePipe(usbHandle, EP_OUT, cmdBufOut, 1, &transferred, NULL);
    if(!trasfert_result) {
        appManager_->Get_UserInterface()->Ui_Print("Err : Spectrometer Init failed.");
        return false;
    }

    Sleep(5000);

    memset(cmdBufOut, 0, CMD_BUF_LENGTH);
    cmdBufOut[0] = CMD_QUERY_INFO;
    trasfert_result = WinUsb_WritePipe(usbHandle, EP_OUT, cmdBufOut, 1, &transferred, NULL);
    if(!trasfert_result) {
        appManager_->Get_UserInterface()->Ui_Print("Err : Spectrometer Querry Info send failed.");
        return false;
    }

    memset(cmdBufIn, 0, CMD_BUF_LENGTH);
    trasfert_result = WinUsb_ReadPipe(usbHandle, EP_IN_CMD, cmdBufIn, CMD_BUF_LENGTH, &transferred, NULL);
    if(!trasfert_result) {
        appManager_->Get_UserInterface()->Ui_Print("Err : Spectrometer Querry Info recived failed.");
        return false;
    }

    is_connected = true;
    return true;
}