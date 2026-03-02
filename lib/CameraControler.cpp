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

CameraControler::CameraControler(AppManager* _appManager_) : appManager_(_appManager_)
{
    new OutputPackage(appManager_, new std::string("creation d'une camera !!!"));
}