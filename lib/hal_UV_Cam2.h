#ifndef HALUVCAM_H
#define HALUVCAM_H

#include <process.h>
#include <conio.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

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

#include <list>

using namespace std;

typedef std::list<PvBuffer *> BufferList;

#define BUFFER_COUNT ( 16 )

///
/// Function Prototypes
///
PvDevice *ConnectToDevice( const PvString &aConnectionID );
PvStream *OpenStream( const PvString &aConnectionID );
void ConfigureStream( PvDevice *aDevice, PvStream *aStream );
void CreateStreamBuffers( PvDevice *aDevice, PvStream *aStream, BufferList *aBufferList );
void AcquireImages( PvDevice *aDevice, PvStream *aStream );
void FreeStreamBuffers( BufferList *aBufferList );

int main_eBus2();

#endif