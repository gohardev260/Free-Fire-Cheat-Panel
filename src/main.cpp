#include <Windows.h>
#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include "gui/GuiStyles.h"
#include "memory/Memory.h"
#include "memory/Scanner.h"
#include "memory/Offsets.h"
#include "security/Security.h"
#include "security/XorStr.h"
#include "security/AntiCrack.h"

// =======================================================================================
// IMPORTANT: YOU MUST DOWNLOAD DEAR IMGUI AND ADD THESE FILES TO YOUR PROJECT
// =======================================================================================
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Global Logic State
bool bAimbot = false;
bool bScope = false;
bool bQuickSwitch = false;
Memory mem;

// Main Code
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // SECURITY: Run Anti-Crack Checks (Debug detection + PE Erasure)
    AntiCrack::RunChecks();

    // SECURITY: Generate Random Title
    std::string windowTitle = Security::RandomString(15);
    
    // Create Application Window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Gohar Xiters"), NULL };
    RegisterClassEx(&wc);
    // Use Random Title for the actual Window
    HWND hwnd = CreateWindow(_T("Gohar Xiters"), windowTitle.c_str(), WS_POPUP, 100, 100, 400, 300, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(_T("Gohar Xiters"), wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Memory Init Thread (Non-blocking)
    std::thread memThread([]() {
        while (!mem.Attach(Offsets::GAME_PROCESS_NAME)) { Sleep(1000); }
        // GuiStyles::AddToast("Connected to Free Fire!", 1); 
    });
    memThread.detach();

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // --- DRAW PANEL ---
        // Create a full-screen window inside our borderless API window for the UI
        // ImGui::SetNextWindowPos(ImVec2(0,0)); 
        // ImGui::SetNextWindowSize(ImVec2(400,300));
        ImGui::Begin("Gohar Xiters", NULL, 0); // Add flags like ImGuiWindowFlags_NoDecoration

        ImGui::TextColored({ 0, 1, 0, 1 }, "Gohar Xiters - Developed by Gohar Rehman");
        
        // Minimize / Close Buttons
        ImGui::BeginGroup();
            if (ImGui::Button("_", ImVec2(30, 20))) { ShowWindow(hwnd, SW_MINIMIZE); }
            ImGui::SameLine();
            if (ImGui::Button("X", ImVec2(30, 20))) { done = true; }
        ImGui::EndGroup();

        ImGui::Text("Status: %s", (mem.processId > 0) ? "CONNECTED" : "WAITING...");

        // CHEATS
        if (ImGui::Checkbox("Aimbot", &bAimbot)) {
            // Logic handled here
            GuiStyles::AddToast(bAimbot ? "Aimbot ON" : "Aimbot OFF", bAimbot ? 1 : 0);
        }
        if (ImGui::Checkbox("Sniper Scope", &bScope)) {
            GuiStyles::AddToast(bScope ? "Scope ON" : "Scope OFF", bScope ? 1 : 0);
        }
        if (ImGui::Checkbox("Quick Switch", &bQuickSwitch)) {
            GuiStyles::AddToast(bQuickSwitch ? "Quick Switch ON" : "Quick Switch OFF", bQuickSwitch ? 1 : 0);
        }

        // DRAW TOASTS
        if (!GuiStyles::toasts.empty()) {
            float yPos = 250.0f; // Start from bottom
            for (auto it = GuiStyles::toasts.begin(); it != GuiStyles::toasts.end(); ) {
                // Draw Toast Window
                ImGui::SetNextWindowPos(ImVec2(20, yPos));
                ImGui::SetNextWindowSize(ImVec2(200, 40));
                ImGui::Begin(("Toast" + it->message).c_str(), 
                    NULL, 
                    1 | 2 | 4 | 32 | 128 // NoTitleBar, NoResize, NoMove, NoScrollbar, NoInputs (Hack using int for flags)
                    // Real flags: ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs
                );
                
                ImVec4 textColor = (it->type == 1) ? ImVec4(0,1,0,1) : ImVec4(1,1,1,1);
                ImGui::TextColored(textColor, it->message.c_str());
                ImGui::End();

                yPos -= 50.0f; // Stack Upwards
                it->timer += ImGui::GetIO().DeltaTime;
                
                if (it->timer > it->duration) {
                    it = GuiStyles::toasts.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        ImGui::End();

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // Transparent
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Should use GetDrawData() result

        g_pSwapChain->Present(1, 0); // Present with vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(_T("Gohar Xiters"), wc.hInstance);

    return 0;
}

// Helper functions (DirectX boilerplate)
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
