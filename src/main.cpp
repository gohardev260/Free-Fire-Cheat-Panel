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

// Forward declarations for UI Helpers (FIXED)
void DragWindow(HWND hwnd);
bool ToggleButton(const char* label, bool* v);

// Global Logic State
bool bAutoLock = false; // F1
bool bRecoil = false;   // F2
bool bMagicBullet = false; // Mouse4
bool bInstantSwitch = false; // F3
bool bQuickScope = false; // CapsLock
bool bEspMaster = true; // Num1
bool bEspSkeleton = true; // Num2
bool bEspTags = false; // Num3
bool bEspLoot = true; // Num4
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
    ImGui_ImplWin32::Init(hwnd);
    ImGui_ImplDX11::Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Memory Init Thread (Non-blocking)
    std::thread memThread([]() {
        while (!mem.Attach(Offsets::GAME_PROCESS_NAME)) { Sleep(1000); }
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
        ImGui_ImplDX11::NewFrame();
        ImGui_ImplWin32::NewFrame();
        ImGui::NewFrame();

        // --- DRAW PANEL ---
        ImGui::SetNextWindowPos(ImVec2(0,0)); 
        ImGui::SetNextWindowSize(ImVec2(450, 400)); // Increased size for new content
        ImGui::Begin("Gohar Panel", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground); 

        // 1. Header Section
        DragWindow(hwnd); // Allow dragging
        
        // Title Row
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "GOHAR"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.0f, 1.0f, 1.0f), "PANEL"); // Purple
        ImGui::SameLine();
        ImGui::TextDisabled("developed by Gohar Rehman");
        
        // Window Controls (Right Align)
        ImGui::SameLine(ImGui::GetWindowWidth() - 60);
        if (ImGui::Button("-", ImVec2(25, 20))) ShowWindow(hwnd, SW_MINIMIZE);
        ImGui::SameLine();
        if (ImGui::Button("x", ImVec2(25, 20))) done = true;

        // Social Buttons
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
        if (ImGui::Button("[Discord]")) { ShellExecute(0, 0, L"https://discord.gg/example", 0, 0 , SW_SHOW); }
        ImGui::SameLine();
        if (ImGui::Button("[WhatsApp]")) { ShellExecute(0, 0, L"https://whatsapp.com/example", 0, 0 , SW_SHOW); }

        ImGui::Spacing();
        ImGui::Separator();

        // 2. System Monitor
        ImGui::BeginChild("SystemMonitor", ImVec2(0, 35), true);
        ImGui::Text("SYSTEM MONITOR");
        ImGui::SameLine(200);
        ImGui::Text("Emulator State: "); ImGui::SameLine();
        if (mem.processId > 0) ImGui::TextColored(ImVec4(0, 1, 0, 1), "[ ONLINE ]");
        else ImGui::TextColored(ImVec4(1, 0, 0, 1), "[ OFFLINE ]");
        ImGui::EndChild();

        // 3. Scrollable Feature Area
        ImGui::BeginChild("Features", ImVec2(0, 0), true); // Fill remaining space

            // SECTION: HEADSHOT & WEAPON CONFIG
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ":: HEADSHOT & WEAPON CONFIG ::");
            ImGui::Separator();
            if (ToggleButton("[ F1    ]   Auto-Lock Precision", &bAutoLock))   GuiStyles::AddToast("Auto-Lock Toggled", 1);
            if (ToggleButton("[ F2    ]   Recoil Control System", &bRecoil))     GuiStyles::AddToast("Recoil Control Toggled", 1);
            if (ToggleButton("[ MOUSE4]   Magic Bullet Tracking", &bMagicBullet)) GuiStyles::AddToast("Magic Bullet Toggled", 1);
            ImGui::Spacing();

            // SECTION: SNIPER ASSIST
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ":: SNIPER ASSIST ::");
            ImGui::Separator();
            if (ToggleButton("[ F3    ]   Instant Switch Logic", &bInstantSwitch)) GuiStyles::AddToast("Instant Switch Toggled", 1);
            if (ToggleButton("[ CAPSLK]   Quick Scope Delay Fix", &bQuickScope))   GuiStyles::AddToast("Quick Scope Toggled", 1);
            ImGui::Spacing();

            // SECTION: VISUALS & ESP
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ":: VISUALS & ESP ::");
            ImGui::Separator();
            if (ToggleButton("[ NUM_1 ]   Master ESP Toggle", &bEspMaster))      GuiStyles::AddToast("ESP Master Toggled", 1);
            if (ToggleButton("[ NUM_2 ]   Skeleton Wireframe", &bEspSkeleton))   GuiStyles::AddToast("Skeleton ESP Toggled", 1);
            if (ToggleButton("[ NUM_3 ]   Distance Tags & Health", &bEspTags))   GuiStyles::AddToast("Tags ESP Toggled", 1);
            if (ToggleButton("[ NUM_4 ]   Loot & Item ESP", &bEspLoot))          GuiStyles::AddToast("Loot ESP Toggled", 1);

        ImGui::EndChild();

        // Keybind Logic (Poll Keys)
        if (GetAsyncKeyState(VK_F1) & 1) { bAutoLock = !bAutoLock; GuiStyles::AddToast(bAutoLock ? "Auto-Lock ON" : "Auto-Lock OFF", bAutoLock ? 1 : 0); }
        if (GetAsyncKeyState(VK_F2) & 1) { bRecoil = !bRecoil; GuiStyles::AddToast(bRecoil ? "Recoil ON" : "Recoil OFF", bRecoil ? 1 : 0); }
        if (GetAsyncKeyState(VK_F3) & 1) { bInstantSwitch = !bInstantSwitch; GuiStyles::AddToast(bInstantSwitch ? "Switch ON" : "Switch OFF", bInstantSwitch ? 1 : 0); }
        if (GetAsyncKeyState(VK_XBUTTON1) & 1) { bMagicBullet = !bMagicBullet; GuiStyles::AddToast(bMagicBullet ? "Magic Bullet ON" : "Magic Bullet OFF", bMagicBullet ? 1 : 0); } // Mouse4
        if (GetAsyncKeyState(VK_CAPITAL) & 1) { bQuickScope = !bQuickScope; GuiStyles::AddToast(bQuickScope ? "Quick Scope ON" : "Quick Scope OFF", bQuickScope ? 1 : 0); }
        if (GetAsyncKeyState(VK_NUMPAD1) & 1) { bEspMaster = !bEspMaster; }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1) { bEspSkeleton = !bEspSkeleton; }
        if (GetAsyncKeyState(VK_NUMPAD3) & 1) { bEspTags = !bEspTags; }
        if (GetAsyncKeyState(VK_NUMPAD4) & 1) { bEspLoot = !bEspLoot; }

        // DRAW TOASTS
        if (!GuiStyles::toasts.empty()) {
            float yPos = 350.0f; // Adjusted for taller window
            for (auto it = GuiStyles::toasts.begin(); it != GuiStyles::toasts.end(); ) {
                ImGui::SetNextWindowPos(ImVec2(20, yPos));
                ImGui::SetNextWindowSize(ImVec2(250, 35));
                ImGui::Begin(("Toast" + it->message).c_str(), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
                ImGui::TextColored(ImVec4(0, 1, 1, 1), it->message.c_str()); // Blue Toast as requested
                ImGui::End();
                
                yPos -= 40.0f;
                it->timer += ImGui::GetIO().DeltaTime;
                if (it->timer > it->duration) it = GuiStyles::toasts.erase(it);
                else ++it;
            }
        }
        
        ImGui::End();

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // Transparent
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11::RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
    }

    // Cleanup
    ImGui_ImplDX11::Shutdown();
    ImGui_ImplWin32::Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(_T("Gohar Xiters"), wc.hInstance);

    return 0;
}

// =======================================================================================
// UI HELPER IMPLEMENTATIONS (FIXED)
// =======================================================================================

// Drag Window Logic (For WS_POPUP)
void DragWindow(HWND hwnd)
{
    // If mouse is left-clicked and dragging on the background
    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
        // Use WinAPI to drag the window (Best Performance)
        ReleaseCapture();
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }
}

// Custom Toggle Button (Gohar Panel Style)
// Renders: [ Label ........................... [ STATUS ] ]
bool ToggleButton(const char* label, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = ImGui::GetContentRegionAvail().x;

    ImGui::InvisibleButton(label, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;

    // Colors
    ImU32 text_col = *v ? IM_COL32(160, 32, 240, 255) : IM_COL32(100, 100, 100, 255); // Purple vs Grey
    ImU32 status_col = *v ? IM_COL32(160, 32, 240, 255) : IM_COL32(80, 80, 80, 255);
    const char* status_text = *v ? "[ ON  ]" : "[ OFF ]";

    // Draw Label
    draw_list->AddText(ImVec2(p.x + 5, p.y + 2), text_col, label);

    // Draw Status (Right Aligned)
    float statusWidth = 60.0f; // Approx width of [ ON ]
    draw_list->AddText(ImVec2(p.x + width - statusWidth, p.y + 2), status_col, status_text);

    return *v; // Return true if state matches? No, standard returns current state usually, but Checkbox returns 'changed'.
               // Here we return *v just for creating if condition, but standard ImGui::Checkbox returns true if CHANGED.
               // My usage in main.cpp is: if (ToggleButton(...)) AddToast. So I should return 'hovered && clicked'.
    // Fix return value to be 'Pressed'
    return ImGui::IsItemClicked(); 
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
