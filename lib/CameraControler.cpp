#include "CameraControler.h"
#include "ThreadSecureQueue.h"
#include "OutputPackage.h"

#include <PvSampleUtils.h>
#include <PvSystem.h>

#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvDeviceU3V.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvStreamU3V.h>
#include <PvBuffer.h>
#include <PvDecompressionFilter.h>

#include <iostream>


CameraControler::CameraControler(AppManager* _appManager_, std::string name, std::string id)
: appManager_(_appManager_)
{
    data.name = name;
    data.id = id;
    data.is_connected = false;
    data.is_streaming = false;
    data.is_recording = false;
    data.fps = DEFAULT_FPS;

    pv_result = new PvResult();
    buffer_list = new std::list<PvBuffer *>();
    thread_acquire = NULL;
}

CameraControler::CameraControler(AppManager* _appManager_)
: CameraControler(_appManager_, "Default_name", "0.0.0.0")
{}

CameraControler::~CameraControler(){
    delete pv_result;
    delete buffer_list;
    delete thread_acquire;
}

s_camera_data CameraControler::Get_Data() {
    return data;
}

void CameraControler::Set_Is_Connected(bool new_state) {
    data.is_connected = new_state;
}

void CameraControler::Set_Is_Streaming(bool new_state) {
    data.is_streaming = new_state;
}

void CameraControler::Set_Is_Recording(bool new_state) {
    data.is_recording = new_state;
}

bool CameraControler::Try_Connection() {    
    bool result = true;
    const PvString deviceID = PvString(data.id.c_str());

    device = PvDevice::CreateAndConnect( deviceID, pv_result);

    if(device == NULL) {
        new OutputPackage(appManager_, new std::string("Err : can't connect"));
        return result = false;
    }
    stream = PvStream::CreateAndOpen( deviceID, pv_result);
    
    if(stream == NULL) {
        new OutputPackage(appManager_, new std::string("Err : can't open stream"));
        return result = false;
    }
    
    device_GEV = dynamic_cast<PvDeviceGEV *>( device );
    
    if(device_GEV == NULL) {
        new OutputPackage(appManager_, new std::string("Err : can't connect with GEV"));
        return result = false;
    }
    
    PvStreamGEV *stream_GEV = static_cast<PvStreamGEV *>( stream );
    
    if(stream_GEV == NULL) {
        new OutputPackage(appManager_, new std::string("Err : can't open GEV stream"));
        return result = false;
    }
    
    device_GEV->NegotiatePacketSize();

    // Configure device streaming destination
    device_GEV->SetStreamDestination( stream_GEV->GetLocalIPAddress(), stream_GEV->GetLocalPort() );
    
    data.is_connected = true;
    return result;
}

void CameraControler::Create_Stream_Buffers()
{
    // Reading payload size from device
    uint32_t lSize = device->GetPayloadSize();
    
    // Use BUFFER_COUNT or the maximum number of buffers, whichever is smaller
    uint32_t buffer_count = ( stream->GetQueuedBufferMaximum() < BUFFER_COUNT ) ? 
    stream->GetQueuedBufferMaximum() :
    BUFFER_COUNT;
    
    // Allocate buffers
    for ( uint32_t i = 0; i < buffer_count; i++ )
    {
        // Create new buffer object
        PvBuffer *buffer = new PvBuffer;
        
        // Have the new buffer object allocate payload memory
        buffer->Alloc( static_cast<uint32_t>( lSize ) );
        
        // Add to external list - used to eventually release the buffers
        buffer_list->push_back( buffer );
    }
    
    // Queue all buffers in the stream
    std::list<PvBuffer *>::iterator lIt = buffer_list->begin();
    while ( lIt != buffer_list->end() )
    {
        stream->QueueBuffer( *lIt );
        lIt++;
    }
}

void CameraControler::Free_Stream_Buffers()
{
    // Go through the buffer list
    std::list<PvBuffer *>::iterator lIt = buffer_list->begin();
    while ( lIt != buffer_list->end() )
    {
        delete *lIt;
        lIt++;
    }

    // Clear the buffer list 
    buffer_list->clear();
}

void CameraControler::Acquire_Images() {
    Create_Stream_Buffers();
    Send_Fps();

    PvGenParameterArray *lDeviceParams = device->GetParameters();

    // Map the GenICam AcquisitionStart and AcquisitionStop commands
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Get stream parameters
    PvGenParameterArray *lStreamParams = stream->GetParameters();

    // Map a few GenICam stream stats counters
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRate" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "Bandwidth" ) );

    device->StreamEnable();
    lStart->Execute();

    data.is_streaming = true;    

    while ((appManager_->Get_Is_Running()) && (data.is_streaming))
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;

        // Retrieve next buffer
        PvResult lResult = stream->RetrieveBuffer( &lBuffer, &lOperationResult, 1000 );

        if (!lResult.IsOK()){
            new OutputPackage(appManager_, new std::string("Err : Fail to recive buffer."));
            continue;
        }

        if (!lOperationResult.IsOK()){
            stream->QueueBuffer( lBuffer );
            new OutputPackage(appManager_, new std::string("Err : Fail operqtion while recive buffer."));
            continue;
        }

        if(lBuffer->GetPayloadType() ==  PvPayloadTypeImage) {
            uint8_t *data   = lBuffer->GetImage()->GetDataPointer();
            uint32_t width  = lBuffer->GetImage()->GetWidth();
            uint32_t height = lBuffer->GetImage()->GetHeight();

            // new OutputPackage(appManager_, new std::string("IMAGE RECIVED"));
            new OutputPackage(appManager_, new std::string(this->data.name), data, width, height);
        }
        else {
            new OutputPackage(appManager_, new std::string("Err : PayloadType not supported."));
        }

        stream->QueueBuffer( lBuffer );
    }

    lStop->Execute();
    device->StreamDisable();
    stream->AbortQueuedBuffers();
    while ( stream->GetQueuedBufferCount() > 0 )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;

        stream->RetrieveBuffer( &lBuffer, &lOperationResult );
    }

    Free_Stream_Buffers();
}

void CameraControler::start_Acquire() {
    thread_acquire = new std::thread([this]() { Acquire_Images(); });
}

void CameraControler::stop_Acquire() {
    data.is_streaming = false;
    if(thread_acquire != NULL){
        thread_acquire->join();
    }
}

int CameraControler::Get_Fps() {
    return data.fps;
}

void CameraControler::Set_Fps(int _fps){
    data.fps = _fps;
    if(data.is_connected){
        Send_Fps();
    }
}

void CameraControler::Send_Fps() {
    if (!data.is_connected || device == nullptr) {
        new OutputPackage(appManager_, new std::string("Err : Device not connected, impossible to send fps."));
        return;
    }

    PvGenParameterArray *params = device->GetParameters();

    // FPS
    PvGenBoolean *lEnable = dynamic_cast<PvGenBoolean *>(
        params->Get("AcquisitionFrameRateEnable")
    );
    if (lEnable) lEnable->SetValue(true);
    
    PvGenBoolean *lEnableMax = dynamic_cast<PvGenBoolean *>(
        params->Get("MaxAcquisitionFrameRateEnable")
    );
    if (lEnableMax) lEnableMax->SetValue(true);

    PvGenFloat *lFPS = dynamic_cast<PvGenFloat *>(
        params->Get("JAIAcquisitionFrameRate")
    );
    if (lFPS) lFPS->SetValue((double)data.fps);
}

void CameraControler::Print_Param() {
    PvGenParameterArray *params = device->GetParameters();
    uint32_t count = params->GetCount();

    for (uint32_t i = 0; i < count; i++) {
        PvGenParameter *param = params->Get(i);
        
        PvString name;
        param->GetName(name);
        
        PvString type;
        // afficher nom et type
        std::cout << name.GetAscii() << std::endl;
    }
}