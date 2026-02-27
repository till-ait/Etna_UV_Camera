#pragma once

#include <windows.h>
#include <string>

// ---------------------------------------------------------------------------
// UVCamDisplay
//
// Fenetre GDI independante pour afficher des images Mono8 en temps reel.
// Chaque instance gere sa propre fenetre et son propre thread de messages.
//
// Usage typique :
//
//   UVCamDisplay cam1("Camera UV 1");
//   UVCamDisplay cam2("Camera UV 2");
//
//   cam1.Open(1280, 1024);
//   cam2.Open(640, 512);
//
//   // Dans la boucle d'acquisition :
//   cam1.PushFrame(pData, width, height);
//   cam2.PushFrame(pData, width, height);
//
//   cam1.Close();
//   cam2.Close();
// ---------------------------------------------------------------------------

class UVCamDisplay
{
public:

    // aTitle : titre de la fenetre (identifie visuellement la camera)
    explicit UVCamDisplay( const std::string &aTitle = "UV Camera" );

    // Le destructeur appelle Close() automatiquement si besoin
    ~UVCamDisplay();

    // Ouvre la fenetre.
    // aInitWidth / aInitHeight : taille initiale de la fenetre en pixels.
    // Retourne true si succes.
    bool Open( int aInitWidth = 800, int aInitHeight = 600 );

    // Affiche un frame Mono8 dans la fenetre.
    // aData   : pointeur vers les pixels (1 octet par pixel)
    // aWidth  : largeur de l'image en pixels
    // aHeight : hauteur de l'image en pixels
    // Thread-safe : peut etre appele depuis n'importe quel thread.
    void PushFrame( const uint8_t *aData, uint32_t aWidth, uint32_t aHeight );

    // Ferme la fenetre et attend la fin du thread.
    void Close();

    // Retourne true si la fenetre est toujours ouverte (non fermee par l'utilisateur)
    bool IsOpen() const;

    // Titre de la fenetre (lecture seule)
    const std::string &GetTitle() const { return mTitle; }

private:

    // Nom unique de la classe Win32 (derive du titre + adresse de l'instance)
    std::string  mClassName;
    std::string  mTitle;

    HWND         mHwnd;         // handle de la fenetre
    HANDLE       mThread;       // thread de la boucle de messages
    HANDLE       mReadyEvent;   // signale quand la fenetre est prete

    // Copie interne du dernier frame pour le redessiner si la fenetre est
    // redimensionnee (WM_PAINT)
    uint8_t     *mFrameBuffer;
    uint32_t     mFrameWidth;
    uint32_t     mFrameHeight;
    CRITICAL_SECTION mFrameLock; // protege mFrameBuffer en acces concurrent

    // Classe de fenetre enregistree ?
    bool         mClassRegistered;

    // --- Internals ---

    // Fonction statique passee a CreateThread (ne peut pas etre membre non-static)
    static DWORD WINAPI  sWindowThread( LPVOID lpParam );

    // Procedure de fenetre statique (recoit WM_PAINT, WM_DESTROY, etc.)
    static LRESULT CALLBACK sWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    // Dessin effectif via StretchDIBits (appele depuis sWndProc)
    void DrawFrame( HDC hdc );

    // Interdit la copie (chaque instance possede son thread/fenetre)
    UVCamDisplay( const UVCamDisplay & )            = delete;
    UVCamDisplay &operator=( const UVCamDisplay & ) = delete;
};