#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <cmath>

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define TRANSPARENT_COLOR RGB(255, 0, 255)

HWND g_hWnd = NULL;
LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pDevice = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CreateTransparentWindow()
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"OverlayDemoClass";

    RegisterClassEx(&wc);

    g_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        L"OverlayDemoClass",
        L"",
        WS_POPUP,
        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!g_hWnd)
        return;

    SetLayeredWindowAttributes(g_hWnd, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
    ShowWindow(g_hWnd, SW_SHOW);
    UpdateWindow(g_hWnd);
}

bool InitD3D()
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D)
        return false;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.EnableAutoDepthStencil = FALSE;

    HRESULT hr = g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        g_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pDevice);

    return SUCCEEDED(hr);
}

void DrawTextCentered(const char* text, int x, int y, DWORD color)
{
    ID3DXFont* font = NULL;
    D3DXCreateFontA(
        g_pDevice,
        30, 0, FW_BOLD, 1, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_QUALITY,
        "Arial",
        &font);

    RECT rect = { x, y, WINDOW_WIDTH, WINDOW_HEIGHT };
    font->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
    font->Release();
}

void DrawCircle(float cx, float cy, float radius, DWORD color, int segments = 64)
{
    struct Vertex
    {
        float x, y, z, rhw;
        DWORD color;
    };

    Vertex vertices[128];
    if (segments > 127)
        segments = 127;

    const float step = (float)(D3DX_PI * 2.0f / segments);

    for (int i = 0; i <= segments; ++i)
    {
        float angle = i * step;
        vertices[i].x = cx + cosf(angle) * radius;
        vertices[i].y = cy + sinf(angle) * radius;
        vertices[i].z = 0.0f;
        vertices[i].rhw = 1.0f;
        vertices[i].color = color;
    }

    g_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, segments, vertices, sizeof(Vertex));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    CreateTransparentWindow();
    if (!g_pDevice && !InitD3D())
    {
        MessageBoxA(NULL, "Direct3D init failed", "Error", MB_OK);
        return 1;
    }

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, TRANSPARENT_COLOR, 1.0f, 0);

        if (SUCCEEDED(g_pDevice->BeginScene()))
        {
            DrawCircle((float)WINDOW_WIDTH / 2.0f, (float)WINDOW_HEIGHT / 2.0f, 90.0f,
                D3DCOLOR_ARGB(255, 255, 0, 0), 64);

            DrawTextCentered("OVERLAY DEMO", 80, 80, D3DCOLOR_ARGB(255, 255, 255, 255));

            g_pDevice->EndScene();
        }

        g_pDevice->Present(NULL, NULL, NULL, NULL);
    }

    if (g_pDevice) g_pDevice->Release();
    if (g_pD3D) g_pD3D->Release();

    return 0;
}
