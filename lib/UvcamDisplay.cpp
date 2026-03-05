#include "UVCamDisplay.h"
#include <stdexcept>
#include <cstring>

// ---------------------------------------------------------------------------
// Constructeur / Destructeur
// ---------------------------------------------------------------------------

UVCamDisplay::UVCamDisplay( const std::string &aTitle )
    : mTitle          ( aTitle )
    , mHwnd           ( NULL )
    , mThread         ( NULL )
    , mReadyEvent     ( NULL )
    , mFrameBuffer    ( nullptr )
    , mFrameWidth     ( 0 )
    , mFrameHeight    ( 0 )
    , mClassRegistered( false )
{
    // Nom de classe unique : titre + adresse de l'instance en hexa
    // => plusieurs instances peuvent coexister sans collision
    char lBuf[64];
    snprintf( lBuf, sizeof(lBuf), "UVCamDisplay_%p", (void*)this );
    mClassName = lBuf;

    InitializeCriticalSection( &mFrameLock );
}

UVCamDisplay::~UVCamDisplay()
{
    Close();
    DeleteCriticalSection( &mFrameLock );
}

// ---------------------------------------------------------------------------
// Open : cree la fenetre dans un thread dedie
// ---------------------------------------------------------------------------

bool UVCamDisplay::Open( int aInitWidth, int aInitHeight )
{
    if ( mHwnd != NULL )
        return true;  // deja ouverte

    // Stocker la taille initiale dans un bloc transmis au thread
    // (le thread libere lui-meme cette memoire)
    struct StartParams { UVCamDisplay *self; int w; int h; };
    StartParams *lParams = new StartParams{ this, aInitWidth, aInitHeight };

    mReadyEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    mThread     = CreateThread( NULL, 0, sWindowThread, lParams, 0, NULL );

    if ( mThread == NULL )
    {
        CloseHandle( mReadyEvent );
        mReadyEvent = NULL;
        delete lParams;
        return false;
    }

    // Attendre que la fenetre soit creee avant de retourner
    WaitForSingleObject( mReadyEvent, INFINITE );
    CloseHandle( mReadyEvent );
    mReadyEvent = NULL;

    return ( mHwnd != NULL );
}

// ---------------------------------------------------------------------------
// Close : ferme la fenetre et attend la fin du thread
// ---------------------------------------------------------------------------

void UVCamDisplay::Close()
{
    if ( mHwnd && IsWindow( mHwnd ) )
    {
        PostMessage( mHwnd, WM_CLOSE, 0, 0 );
    }

    if ( mThread )
    {
        WaitForSingleObject( mThread, 5000 );
        CloseHandle( mThread );
        mThread = NULL;
    }

    // Liberer le buffer de frame
    EnterCriticalSection( &mFrameLock );
    delete[] mFrameBuffer;
    mFrameBuffer = nullptr;
    mFrameWidth  = 0;
    mFrameHeight = 0;
    LeaveCriticalSection( &mFrameLock );

    mHwnd = NULL;
}

// ---------------------------------------------------------------------------
// IsOpen : verifie si la fenetre est toujours vivante
// ---------------------------------------------------------------------------

bool UVCamDisplay::IsOpen() const
{
    return ( mHwnd != NULL && IsWindow( mHwnd ) );
}

// ---------------------------------------------------------------------------
// PushFrame : copie le frame et demande un redessin
// ---------------------------------------------------------------------------

void UVCamDisplay::PushFrame( const uint8_t *aData, uint32_t aWidth, uint32_t aHeight )
{
    if ( !IsOpen() || aData == nullptr || aWidth == 0 || aHeight == 0 )
        return;

    uint32_t lSize = aWidth * aHeight;  // Mono8 : 1 octet par pixel

    EnterCriticalSection( &mFrameLock );

    // Reallocation uniquement si la taille a change
    if ( aWidth != mFrameWidth || aHeight != mFrameHeight )
    {
        delete[] mFrameBuffer;
        mFrameBuffer = new uint8_t[ lSize ];
        mFrameWidth  = aWidth;
        mFrameHeight = aHeight;
    }

    // Copie du buffer camera (aData sera re-queue des le retour de PushFrame)
    memcpy( mFrameBuffer, aData, lSize );

    LeaveCriticalSection( &mFrameLock );

    // Demander a Windows de redessiner la fenetre → provoque WM_PAINT
    InvalidateRect( mHwnd, NULL, FALSE );
}

// ---------------------------------------------------------------------------
// DrawFrame : dessin GDI (appele depuis sWndProc lors de WM_PAINT)
// ---------------------------------------------------------------------------

void UVCamDisplay::DrawFrame( HDC hdc )
{
    EnterCriticalSection( &mFrameLock );

    if ( mFrameBuffer == nullptr || mFrameWidth == 0 || mFrameHeight == 0 )
    {
        LeaveCriticalSection( &mFrameLock );
        return;
    }

    // --- Construire le BITMAPINFO pour une image Mono8 ---
    // La palette de 256 entrees suit immediatement le header
    struct
    {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD          bmiColors[256];
    } bmi = {};

    bmi.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER );
    bmi.bmiHeader.biWidth       = static_cast<LONG>( mFrameWidth );
    bmi.bmiHeader.biHeight      = -static_cast<LONG>( mFrameHeight ); // top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 8;       // Mono8
    bmi.bmiHeader.biCompression = BI_RGB;  // pas de compression

    // Palette niveaux de gris : index i → couleur (i, i, i)
    for ( int i = 0; i < 256; i++ )
    {
        bmi.bmiColors[i].rgbRed   = static_cast<BYTE>( i );
        bmi.bmiColors[i].rgbGreen = static_cast<BYTE>( i );
        bmi.bmiColors[i].rgbBlue  = static_cast<BYTE>( i );
    }

    // Taille de la zone cliente (s'adapte au redimensionnement)
    RECT rcClient;
    GetClientRect( mHwnd, &rcClient );
    int destW = rcClient.right  - rcClient.left;
    int destH = rcClient.bottom - rcClient.top;

    // Dessin avec scaling automatique source → destination
    StretchDIBits(
        hdc,
        0, 0, destW, destH,                          // rectangle destination
        0, 0, mFrameWidth, mFrameHeight,              // rectangle source
        mFrameBuffer,
        reinterpret_cast<BITMAPINFO *>( &bmi ),
        DIB_RGB_COLORS,
        SRCCOPY );

    LeaveCriticalSection( &mFrameLock );
}

// ---------------------------------------------------------------------------
// sWindowThread : thread statique qui cree et gere la fenetre
// ---------------------------------------------------------------------------

DWORD WINAPI UVCamDisplay::sWindowThread( LPVOID lpParam )
{
    // Recuperer les parametres passes par Open()
    struct StartParams { UVCamDisplay *self; int w; int h; };
    StartParams *lParams = static_cast<StartParams *>( lpParam );

    UVCamDisplay *lSelf   = lParams->self;
    int           lWidth  = lParams->w;
    int           lHeight = lParams->h;
    delete lParams;  // liberer la memoire allouee dans Open()

    HINSTANCE hInst = GetModuleHandle( NULL );

    // --- Enregistrement de la classe de fenetre ---
    WNDCLASS wc         = {};
    wc.lpfnWndProc      = UVCamDisplay::sWndProc;  // notre procedure custom
    wc.hInstance        = hInst;
    wc.lpszClassName    = lSelf->mClassName.c_str();
    wc.hbrBackground    = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wc.hCursor          = LoadCursor( NULL, IDC_ARROW );

    if ( RegisterClass( &wc ) )
        lSelf->mClassRegistered = true;

    // --- Creation de la fenetre ---
    // On passe le pointeur 'this' via CreateWindowEx pour le recuperer dans sWndProc
    lSelf->mHwnd = CreateWindowEx(
        0,
        lSelf->mClassName.c_str(),
        lSelf->mTitle.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        lWidth, lHeight,
        NULL, NULL,
        hInst,
        lSelf );   // <-- userData : recupere dans WM_CREATE via CREATESTRUCT
    
    // Signaler a Open() que la fenetre est prete (ou que la creation a echoue)
    SetEvent( lSelf->mReadyEvent );

    if ( lSelf->mHwnd == NULL )
        return 1;

    // --- Boucle de messages ---
    MSG msg;
    while ( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    // Desenregistrer la classe apres destruction de la fenetre
    if ( lSelf->mClassRegistered )
    {
        UnregisterClass( lSelf->mClassName.c_str(), hInst );
        lSelf->mClassRegistered = false;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// sWndProc : procedure de fenetre statique
// ---------------------------------------------------------------------------

LRESULT CALLBACK UVCamDisplay::sWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    UVCamDisplay *lSelf = nullptr;

    if ( uMsg == WM_CREATE )
    {
        // A la creation, Windows passe le lpParam de CreateWindowEx dans CREATESTRUCT
        CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT *>( lParam );
        lSelf = static_cast<UVCamDisplay *>( cs->lpCreateParams );

        // Stocker le pointeur dans la fenetre pour les messages suivants
        SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( lSelf ) );
    }
    else
    {
        // Pour tous les autres messages, recuperer le pointeur stocke
        lSelf = reinterpret_cast<UVCamDisplay *>( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
    }

    switch ( uMsg )
    {
    case WM_PAINT:
        {
            // WM_PAINT est envoye par InvalidateRect() dans PushFrame()
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint( hWnd, &ps );  // obligatoire pour valider la zone
            if ( lSelf )
                lSelf->DrawFrame( hdc );
            EndPaint( hWnd, &ps );              // obligatoire apres BeginPaint
        }
        return 0;

    case WM_SIZE:
        // Forcer un redessin quand l'utilisateur redimensionne la fenetre
        InvalidateRect( hWnd, NULL, FALSE );
        return 0;

    case WM_DESTROY:
        // Envoyer WM_QUIT pour terminer GetMessage() dans sWindowThread
        PostQuitMessage( 0 );
        return 0;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}