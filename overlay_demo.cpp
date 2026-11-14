#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>

// Configuration
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define TRANSPARENT_COLOR RGB(255, 0, 255) // Magenta pour la transparence

HWND hwndOverlay;
LPDIRECT3DDEVICE9 d3ddev = nullptr;

// Fonction pour créer la fenêtre transparente
void CreateOverlay() {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "CS2Overlay";
    RegisterClass(&wc);

    hwndOverlay = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        "CS2Overlay", NULL, WS_POPUP,
        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    // Rendre le magenta transparent
    SetLayeredWindowAttributes(hwndOverlay, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
    ShowWindow(hwndOverlay, SW_SHOW);
}

// Fonction d'initialisation DirectX
bool InitD3D() {
    LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    HRESULT hr = d3d->CreateDevice(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwndOverlay,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev
    );
    
    d3d->Release();
    return SUCCEEDED(hr);
}

// Fonction pour dessiner du texte simple
void DrawText(const char* text, int x, int y) {
    ID3DXFont* font;
    D3DXCreateFont(d3ddev, 24, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                   OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_QUALITY, 
                   "Arial", &font);
    
    RECT rect = {x, y, 0, 0};
    font->DrawText(NULL, text, -1, &rect, DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 255, 255));
    font->Release();
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    CreateOverlay();
    if (!InitD3D()) return 1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Clear with transparent color
        d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, TRANSPARENT_COLOR, 1.0f, 0);
        
        if (SUCCEEDED(d3ddev->BeginScene())) {
            DrawText("CHEAT ACTIVE", 100, 100);
            DrawText("Aimbot: ON", 100, 140);
            
            d3ddev->EndScene();
        }
        d3ddev->Present(NULL, NULL, NULL, NULL);
    }
    return 0;
}
