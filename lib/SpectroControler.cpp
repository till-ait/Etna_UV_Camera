#include "SpectroControler.h"
#include "AppManager.h"
#include "UserInterface.h"
#include <cmath>



SpectroControler::SpectroControler(AppManager *_appManager_) {
    appManager_ = _appManager_;
    is_connected = false;
    shortPacket = 0;
    timeout = 2000;
    transferred = 0;
    trasfert_result = FALSE;
    memset(cmdBufOut, 0, CMD_BUF_LENGTH);
    memset(cmdBufIn, 0, CMD_BUF_LENGTH);
    memset(dataBuf, 0, DATA_BUF_LENGTH);
    integration_time = DEFAULT_INTEGRATION_TIME;
    scans_average = DEFAULT_AVERAGING;
    thread_acquire = NULL;
    is_streaming = false;
    serial_number = "Unknown";
    HANDLE deviceHandle = INVALID_HANDLE_VALUE;
    usbHandleInitialized = false;
}

SpectroControler::~SpectroControler() {
    stop_Acquire();
    delete thread_acquire;
    
    if(usbHandleInitialized){
        WinUsb_Free(usbHandle);
    }

    if(deviceHandle != INVALID_HANDLE_VALUE){
        CloseHandle(deviceHandle);
    }
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
    usbHandleInitialized = true;

    shortPacket = 0;
    WinUsb_SetPipePolicy(usbHandle, 0x01, SHORT_PACKET_TERMINATE, sizeof(shortPacket), &shortPacket);
    WinUsb_SetPipePolicy(usbHandle, 0x02, SHORT_PACKET_TERMINATE, sizeof(shortPacket), &shortPacket);
    shortPacket = 1;
    WinUsb_SetPipePolicy(usbHandle, EP_IN_DATA_1, SHORT_PACKET_TERMINATE, sizeof(shortPacket), &shortPacket);  // ← clé
    WinUsb_SetPipePolicy(usbHandle, EP_IN_DATA_2, SHORT_PACKET_TERMINATE, sizeof(shortPacket), &shortPacket);

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

    Sleep(2000);

    memset(cmdBufOut, 0, CMD_BUF_LENGTH);
    cmdBufOut[0] = CMD_QUERY_INFO;
    cmdBufOut[1] = 0x00;
    trasfert_result = WinUsb_WritePipe(usbHandle, EP_OUT, cmdBufOut, 2, &transferred, NULL);
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

    // serial_number = std::string(reinterpret_cast<char*>(cmdBufIn + 2), 15);
    // std::cout << "Serial : " << serial_number << std::endl;
    std::ostringstream oss;
    for (int i = 2; i < 2 + 15; i++) {
        oss << std::hex << std::uppercase
            << std::setw(2) << std::setfill('0')
            << static_cast<int>(cmdBufIn[i]);
    }
    serial_number = oss.str();

    is_connected = true;

    Set_integration_time(integration_time);
    Set_scans_to_average(scans_average);
    Set_trigger_mode(DEFAULT_TRIGGER_MODE);
    LoadCalibration();

    start_Acquire();

    return true;
}

bool SpectroControler::Set_integration_time(uint32_t micros) {

    if((micros < MIN_INTEGRATION_TIME) || (micros > MAX_INTEGRATION_TIME)) {
        return false;
    }

    integration_time = micros;

    if(!is_connected) {
        return false;
    }

    uint8_t cmd[5];
    cmd[0] = CMD_SET_INTEG_TIME;
    cmd[1] = (integration_time >>  0) & 0xFF;
    cmd[2] = (integration_time >>  8) & 0xFF;
    cmd[3] = (integration_time >> 16) & 0xFF;
    cmd[4] = (integration_time >> 24) & 0xFF;
    return WinUsb_WritePipe(usbHandle, EP_OUT, cmd, 5, &transferred, NULL);
}

bool SpectroControler::Set_scans_to_average(uint16_t scans)
{
    scans_average = scans;

    if(!is_connected) {
        return false;
    }
    
    uint8_t cmd[3];
    cmd[0] = CMD_SET_SCANS_AVG;
    cmd[1] = (scans_average >> 0) & 0xFF;
    cmd[2] = (scans_average >> 8) & 0xFF;
    return WinUsb_WritePipe(usbHandle, EP_OUT, cmd, 3, &transferred, NULL);
}

bool SpectroControler::Set_trigger_mode(uint8_t mode) {
    uint8_t cmd[2];
    cmd[0] = CMD_TRIGGER_MODE;
    cmd[1] = mode;
    return WinUsb_WritePipe(usbHandle, EP_OUT, cmd, 2, &transferred, NULL);
}

void SpectroControler::Get_spectrum() {
    while((appManager_->Get_Is_Running()) && (is_streaming)) {
        uint8_t cmd[1] = { CMD_REQUEST_SPECTRUM };
        WinUsb_WritePipe(usbHandle, EP_OUT, cmd, 1, &transferred, NULL);
        Sleep(integration_time/1000.0 * scans_average + 10);

        // pixels 16-bit little-endian
        int total_bytes = SPECTRUM_PIXELS * 2;
        std::vector<uint8_t> raw(total_bytes*2, 0);
        int received = 0;

        // while (received < total_bytes) {
        //     uint8_t buf[512] = {};
        //     ULONG n = 0;
        //     // if (!WinUsb_ReadPipe(hWinUsb, EP_IN,
        //     //         buf, sizeof(buf), &n, NULL))
        //     //     break;

        //     if(!WinUsb_ReadPipe(usbHandle, EP_IN_DATA_1, buf, sizeof(buf), &n, NULL)){ // TODO peut etre 
        //         std::cout << "BREAK" << std::endl;
        //         break;
        //     }

        //     // Paquet de synchronisation final (0x69)
        //     if (n == 1 && buf[0] == 0x69) {
        //         std::cout << "SYNC" << std::endl;
        //         break;
        //     }

        //     int to_copy = (int)std::min((int)n, total_bytes - received);
        //     std::memcpy(raw.data() + received, buf, to_copy);
        //     received += to_copy;

        //     std::cout << "Recived : " << received << std::endl;
        // }

        // // read synchro byte
        // {
        //     uint8_t sync[1] = {};
        //     ULONG dummy = 0;
        //     // WinUsb_ReadPipe(hWinUsb, EP_IN, sync, 1, &dummy, NULL);
        //     WinUsb_ReadPipe(usbHandle, EP_IN_DATA_1, sync, 1, &dummy, NULL);
        // }

        int i = 0;
        int end_point = EP_IN_DATA_2;
        bool synch_byte_recived = false;
        while(!synch_byte_recived) {
            uint8_t buf[512] = {};
            ULONG n = 0;
            // if (!WinUsb_ReadPipe(hWinUsb, EP_IN,
            //         buf, sizeof(buf), &n, NULL))
            //     break;

            if(i==4) {
                end_point = EP_IN_DATA_1;
            }

            if(!WinUsb_ReadPipe(usbHandle, end_point, buf, sizeof(buf), &n, NULL)){ // TODO peut etre 
                // std::cout << "BREAK" << std::endl;
                break;
            }

            // Paquet de synchronisation final (0x69)
            if (n == 1 && buf[0] == 0x69) {
                // std::cout << "SYNC" << std::endl;
                synch_byte_recived = true;
            } else {
                int to_copy = (int)std::min((int)n, total_bytes*2 - received);
                std::memcpy(raw.data() + received, buf, to_copy);
                received += to_copy;
                // std::cout << "BUF " << (int)buf[0] << std::endl;
            }
            
            // std::cout << "Recived : " << received << std::endl;
            i++;
        }


        // std::cout << (uint16_t)(raw[2*2] | (raw[2*2+1] << 8)) << std::endl;

        // uint16 LE → double
        std::vector<double> spectrum(received/2);
        std::vector<double> wavelengths, intensities;
        for (int i = 0; i < received/2; ++i) {
            uint16_t val = (uint16_t)(raw[2*i] | (raw[2*i+1] << 8));
            // spectrum[i] = static_cast<double> (val);
            // spectrum[i] = 20*(std::log10(static_cast<double> (val)));
            average_spectrum[i] = (1-alpha_coef)*average_spectrum[i] + (alpha_coef)*val;
        }

        ProcessSpectrum(average_spectrum, wavelengths, intensities);
        // std::vector<double> spectrum(SPECTRUM_PIXELS);
        // for (int i = 0; i < SPECTRUM_PIXELS; ++i) {
        //     uint16_t val = (uint16_t)(raw[2*i] | (raw[2*i+1] << 8));
        //     spectrum[i] = static_cast<double>(val);
        // }
        
        if((appManager_->Get_Is_Running()) && (is_streaming)){
            appManager_->Get_UserInterface()->Push_Spectrum(intensities,wavelengths);
        }
    }
}

bool SpectroControler::start_Acquire() {
    if(!is_connected) {
        return false;
    }
    
    is_streaming = true;

    thread_acquire = new std::thread([this]() { Get_spectrum(); });

    return true;
}

void SpectroControler::stop_Acquire() {
    is_streaming = false;
    if(thread_acquire != NULL) {
        thread_acquire->join();
    }
}

std::string SpectroControler::ReadEEPROM(uint8_t slot) {
    uint8_t cmd[2] = { CMD_QUERY_EEPROM, slot };
    WinUsb_WritePipe(usbHandle, EP_OUT, cmd, 2, &transferred, NULL);

    uint8_t resp[18] = {};
    ULONG n = 0;
    WinUsb_ReadPipe(usbHandle, EP_IN_CMD, resp, 18, &n, NULL);

    return std::string(reinterpret_cast<char*>(resp + 2), 15);
}

void SpectroControler::LoadCalibration() {
    std::string s0 = ReadEEPROM(1);  // λ0 (intercept)
    std::string s1 = ReadEEPROM(2);  // C1 nm/pixel
    std::string s2 = ReadEEPROM(3);  // C2 nm/pixel²
    std::string s3 = ReadEEPROM(4);  // C3 nm/pixel³

    wl_coef[0] = std::stod(s0);
    wl_coef[1] = std::stod(s1);
    wl_coef[2] = std::stod(s2);
    wl_coef[3] = std::stod(s3);

    // std::cout << "λ0=" << wl_coef[0] << " C1=" << wl_coef[1]
    //           << " C2=" << wl_coef[2] << " C3=" << wl_coef[3] << std::endl;
}

double SpectroControler::PixelToWavelength(int p) {
    return wl_coef[0]
         + wl_coef[1] * p
         + wl_coef[2] * p * p
         + wl_coef[3] * p * p * p;
}

void SpectroControler::ProcessSpectrum(
    const std::vector<double>& raw,
    std::vector<double>& wavelengths,
    std::vector<double>& intensities)
{
    double dark_avg = 0.0;
    for (int i = 2; i <= 24; i++)
        dark_avg += raw[i];
    dark_avg /= 23.0;

    int n_active = 2047 - 26 + 1;  // = 2022 pixels
    wavelengths.resize(n_active);
    intensities.resize(n_active);

    for (int i = 0; i < n_active; i++) {
        int pixel = 26 + i;
        wavelengths[i] = PixelToWavelength(pixel);
        intensities[i] = std::max(0.0, raw[pixel] - dark_avg);
    }
}

int SpectroControler::Get_integration_time() {
    return integration_time;
}

int SpectroControler::Get_scans_to_average() {
    return scans_average;
}

std::string SpectroControler::Get_serial_number() {
    return serial_number;
}