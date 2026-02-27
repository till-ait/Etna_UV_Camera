#include "hal_UV_Cam.h"

PV_INIT_SIGNAL_HANDLER();

// ---------------------------------------------------------------------------
// Globals pour la fenetre GDI (partagee entre le thread camera et le thread UI)
// ---------------------------------------------------------------------------
HWND   g_hWnd      = NULL;
HANDLE g_hWndReady = NULL;   // event signale quand la fenetre est prete

// ---------------------------------------------------------------------------
// Thread dedie a la fenetre Win32 (message loop obligatoire sous Windows)
// ---------------------------------------------------------------------------
DWORD WINAPI WindowThread( LPVOID lpParam )
{
    HINSTANCE hInst = GetModuleHandle( NULL );

    // Enregistrer la classe de fenetre
    WNDCLASS wc      = {};
    wc.lpfnWndProc   = DefWindowProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = TEXT( "UVCamWindow" );
    wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    RegisterClass( &wc );

    // Creer la fenetre
    g_hWnd = CreateWindowEx(
        0,
        TEXT( "UVCamWindow" ),
        TEXT( "UV Camera - Mono8" ),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, hInst, NULL );

    // Signaler que la fenetre est prete
    SetEvent( g_hWndReady );

    // Boucle de messages
    MSG msg;
    while ( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Affiche un buffer Mono8 dans la fenetre GDI via StretchDIBits
// ---------------------------------------------------------------------------
void DisplayMono8( uint8_t *aData, uint32_t aWidth, uint32_t aHeight )
{
    if ( g_hWnd == NULL || aData == NULL )
        return;

    // Construire un BITMAPINFO pour une image en niveaux de gris 8 bpp
    // La palette de 256 entrees est placee juste apres le BITMAPINFOHEADER
    struct
    {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD          bmiColors[256];
    } bmi = {};

    bmi.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER );
    bmi.bmiHeader.biWidth       = static_cast<LONG>( aWidth );
    bmi.bmiHeader.biHeight      = -static_cast<LONG>( aHeight ); // negatif = top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 8;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Remplir la palette avec les 256 niveaux de gris
    for ( int i = 0; i < 256; i++ )
    {
        bmi.bmiColors[i].rgbRed   = static_cast<BYTE>( i );
        bmi.bmiColors[i].rgbGreen = static_cast<BYTE>( i );
        bmi.bmiColors[i].rgbBlue  = static_cast<BYTE>( i );
    }

    // Recuperer les dimensions de la zone cliente
    RECT rcClient;
    GetClientRect( g_hWnd, &rcClient );
    int destW = rcClient.right  - rcClient.left;
    int destH = rcClient.bottom - rcClient.top;

    HDC hdc = GetDC( g_hWnd );

    // Dessiner l'image en l'etirant a la taille de la fenetre
    StretchDIBits(
        hdc,
        0, 0, destW, destH,          // destination
        0, 0, aWidth, aHeight,        // source
        aData,
        reinterpret_cast<BITMAPINFO *>( &bmi ),
        DIB_RGB_COLORS,
        SRCCOPY );

    ReleaseDC( g_hWnd, hdc );
}

// ---------------------------------------------------------------------------

int main_eBus()
{
    // Demarrer le thread de la fenetre avant la camera
    g_hWndReady = CreateEvent( NULL, TRUE, FALSE, NULL );
    HANDLE hThread = CreateThread( NULL, 0, WindowThread, NULL, 0, NULL );
    WaitForSingleObject( g_hWndReady, INFINITE );   // attendre que la fenetre soit prete
    CloseHandle( g_hWndReady );

    PvDevice *lDevice = NULL;
    PvStream *lStream = NULL;
    BufferList lBufferList;

    cout << "PvStreamSample:" << endl << endl;

    PvString lConnectionID;
    if ( PvSelectDevice( &lConnectionID ) )
    {
        lDevice = ConnectToDevice( lConnectionID );
        if ( NULL != lDevice )
        {
            lStream = OpenStream( lConnectionID );
            if ( NULL != lStream )
            {
                ConfigureStream( lDevice, lStream );
                CreateStreamBuffers( lDevice, lStream, &lBufferList );
                AcquireImages( lDevice, lStream );
                FreeStreamBuffers( &lBufferList );

                cout << "Closing stream" << endl;
                lStream->Close();
                PvStream::Free( lStream );
            }

            cout << "Disconnecting device" << endl;
            lDevice->Disconnect();
            PvDevice::Free( lDevice );
        }
    }

    // Fermer la fenetre proprement
    if ( g_hWnd )
        PostMessage( g_hWnd, WM_CLOSE, 0, 0 );
    WaitForSingleObject( hThread, 3000 );
    CloseHandle( hThread );

    cout << endl << "<press a key to exit>" << endl;
    return 0;
}

// ---------------------------------------------------------------------------

PvDevice *ConnectToDevice( const PvString &aConnectionID )
{
    PvDevice *lDevice;
    PvResult lResult;

    cout << "Connecting to device." << endl;
    lDevice = PvDevice::CreateAndConnect( aConnectionID, &lResult );
    if ( lDevice == NULL )
    {
        cout << "Unable to connect to device: "
             << lResult.GetCodeString().GetAscii()
             << " ("
             << lResult.GetDescription().GetAscii()
             << ")" << endl;
    }
    return lDevice;
}

PvStream *OpenStream( const PvString &aConnectionID )
{
    PvStream *lStream;
    PvResult lResult;

    cout << "Opening stream from device." << endl;
    lStream = PvStream::CreateAndOpen( aConnectionID, &lResult );
    if ( lStream == NULL )
    {
        cout << "Unable to stream from device. "
             << lResult.GetCodeString().GetAscii()
             << " ("
             << lResult.GetDescription().GetAscii()
             << ")" << endl;
    }
    return lStream;
}

void ConfigureStream( PvDevice *aDevice, PvStream *aStream )
{
    PvDeviceGEV *lDeviceGEV = dynamic_cast<PvDeviceGEV *>( aDevice );
    if ( lDeviceGEV != NULL )
    {
        PvStreamGEV *lStreamGEV = static_cast<PvStreamGEV *>( aStream );
        lDeviceGEV->NegotiatePacketSize();
        lDeviceGEV->SetStreamDestination( lStreamGEV->GetLocalIPAddress(), lStreamGEV->GetLocalPort() );
    }
}

void CreateStreamBuffers( PvDevice *aDevice, PvStream *aStream, BufferList *aBufferList )
{
    uint32_t lSize = aDevice->GetPayloadSize();
    uint32_t lBufferCount = ( aStream->GetQueuedBufferMaximum() < BUFFER_COUNT ) ?
        aStream->GetQueuedBufferMaximum() : BUFFER_COUNT;

    for ( uint32_t i = 0; i < lBufferCount; i++ )
    {
        PvBuffer *lBuffer = new PvBuffer;
        lBuffer->Alloc( static_cast<uint32_t>( lSize ) );
        aBufferList->push_back( lBuffer );
    }

    BufferList::iterator lIt = aBufferList->begin();
    while ( lIt != aBufferList->end() )
    {
        aStream->QueueBuffer( *lIt );
        lIt++;
    }
}

void AcquireImages( PvDevice *aDevice, PvStream *aStream )
{
    PvGenParameterArray *lDeviceParams = aDevice->GetParameters();

    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    PvGenCommand *lStop  = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    PvGenParameterArray *lStreamParams = aStream->GetParameters();
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRate" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "Bandwidth" ) );

    cout << "Enabling streaming and sending AcquisitionStart command." << endl;
    aDevice->StreamEnable();
    lStart->Execute();

    char lDoodle[] = "|\\-|-/";
    int  lDoodleIndex = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;
    int    lErrors = 0;
    bool   lRunning = true;

    PvDecompressionFilter lDecompressionFilter;

    cout << endl << "<fermer la fenetre ou appuyer sur 'q' dans la console pour arreter>" << endl;

    while ( lRunning )
    {
        // Quitter si la fenetre a ete fermee
        if ( g_hWnd && !IsWindow( g_hWnd ) )
        {
            lRunning = false;
            break;
        }

        // Quitter si 'q' est appuye dans la console
        if ( _kbhit() && _getch() == 'q' )
        {
            lRunning = false;
            break;
        }

        PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;

        PvResult lResult = aStream->RetrieveBuffer( &lBuffer, &lOperationResult, 1000 );
        if ( lResult.IsOK() )
        {
            if ( lOperationResult.IsOK() )
            {
                lFrameRate->GetValue( lFrameRateVal );
                lBandwidth->GetValue( lBandwidthVal );

                cout << fixed << setprecision( 1 );
                cout << lDoodle[ lDoodleIndex ];
                cout << " BlockID: " << uppercase << hex << setfill( '0' ) << setw( 16 ) << lBuffer->GetBlockID();

                switch ( lBuffer->GetPayloadType() )
                {
                case PvPayloadTypeImage:
                    {
                        PvImage  *lImage  = lBuffer->GetImage();
                        uint32_t  lWidth  = lImage->GetWidth();
                        uint32_t  lHeight = lImage->GetHeight();
                        uint8_t  *lData   = lImage->GetDataPointer();

                        // Affichage GDI natif - Mono8
                        DisplayMono8( lData, lWidth, lHeight );

                        cout << "  W: " << dec << lWidth << " H: " << lHeight;
                    }
                    break;

                case PvPayloadTypeChunkData:
                    cout << " Chunk Data payload type with " << lBuffer->GetChunkCount() << " chunks";
                    break;

                case PvPayloadTypeRawData:
                    cout << " Raw Data with " << lBuffer->GetRawData()->GetPayloadLength() << " bytes";
                    break;

                case PvPayloadTypeMultiPart:
                    cout << " Multi Part with " << lBuffer->GetMultiPartContainer()->GetPartCount() << " parts";
                    break;

                case PvPayloadTypePleoraCompressed:
                    {
                        PvPixelType lPixelType = PvPixelUndefined;
                        uint32_t lWidth = 0, lHeight = 0;
                        PvDecompressionFilter::GetOutputFormatFor( lBuffer, lPixelType, lWidth, lHeight );
                        uint32_t lCalculatedSize = PvImage::GetPixelSize( lPixelType ) * lWidth * lHeight / 8;

                        PvBuffer lDecompressedBuffer;
                        if ( lDecompressionFilter.IsCompressed( lBuffer ) )
                        {
                            lResult = lDecompressionFilter.Execute( lBuffer, &lDecompressedBuffer );
                            if ( !lResult.IsOK() ) break;
                        }

                        uint32_t lDecompressedSize = lDecompressedBuffer.GetSize();
                        if ( lDecompressedSize != lCalculatedSize ) lErrors++;

                        double lCompressionRatio = static_cast<double>( lDecompressedSize ) /
                                                   static_cast<double>( lBuffer->GetAcquiredSize() );
                        cout << dec << " Pleora compressed. Ratio: " << lCompressionRatio
                             << " Errors: " << lErrors
                             << " W: " << lWidth << " H: " << lHeight;
                    }
                    break;

                default:
                    cout << " Payload type not supported";
                    break;
                }

                cout << "  " << lFrameRateVal << " FPS  " << ( lBandwidthVal / 1000000.0 ) << " Mb/s   \r";
            }
            else
            {
                cout << lDoodle[ lDoodleIndex ] << " " << lOperationResult.GetCodeString().GetAscii() << "\r";
            }

            aStream->QueueBuffer( lBuffer );
        }
        else
        {
            cout << lDoodle[ lDoodleIndex ] << " " << lResult.GetCodeString().GetAscii() << "\r";
        }

        ++lDoodleIndex %= 6;
    }

    cout << endl << endl;

    cout << "Sending AcquisitionStop command to the device" << endl;
    lStop->Execute();

    cout << "Disable streaming on the controller." << endl;
    aDevice->StreamDisable();

    cout << "Aborting buffers still in stream" << endl;
    aStream->AbortQueuedBuffers();
    while ( aStream->GetQueuedBufferCount() > 0 )
    {
        PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
        aStream->RetrieveBuffer( &lBuffer, &lOperationResult );
    }
}

void FreeStreamBuffers( BufferList *aBufferList )
{
    BufferList::iterator lIt = aBufferList->begin();
    while ( lIt != aBufferList->end() )
    {
        delete *lIt;
        lIt++;
    }
    aBufferList->clear();
}