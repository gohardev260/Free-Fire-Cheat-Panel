#define _WIN32_WINNT 0x0601
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
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
    // AntiCrack::RunChecks(); // DISABLED: Causes crash on some systems due to PE erasure before window creation

    // SECURITY: Generate Random Title
    std::string windowTitle = Security::RandomString(15);
    
    // Create Application Window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Gohar Xiters"), NULL };
    RegisterClassEx(&wc);
    // Use Random Title for the actual Window
    std::wstring wWindowTitle(windowTitle.begin(), windowTitle.end());
    HWND hwnd = CreateWindow(_T("Gohar Xiters"), wWindowTitle.c_str(), WS_POPUP, 100, 100, 400, 300, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(_T("Gohar Xiters"), wc.hInstance);
        MessageBox(NULL, _T("Failed to create Direct3D Device."), _T("Error"), MB_ICONERROR);
        return 1;
    }

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // DWM Transparency
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    // --- MODERN THEME SETUP ---
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 9.0f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f); // Dark translucent
    style.Colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.7f, 0.0f, 1.0f); // Green Click
    style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Memory Init Thread
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

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // --- MODERN UI LAYOUT ---
        ImGui::SetNextWindowSize(ImVec2(500, 350));
        // Using "Gohar Xiters" as ID but hiding title bar for custom header
        ImGui::Begin("Gohar Xiters", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        // 1. HEADER (Draggable)
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 500, p.y + 50), IM_COL32(20, 20, 20, 255), 12.0f, ImDrawFlags_RoundCornersTop);
        
        ImGui::SetCursorPos(ImVec2(20, 15));
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ GOHAR XITERS ]");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Free Fire External");

        // Close Button
        ImGui::SetCursorPos(ImVec2(460, 12));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
        if (ImGui::Button("X", ImVec2(30, 30))) done = true;
        ImGui::PopStyleColor();

        // Drag Logic: If mouse is in header and clicked
        if (ImGui::IsMouseHoveringRect(p, ImVec2(p.x + 450, p.y + 50)) && ImGui::IsMouseClicked(0)) {
             DragWindow(hwnd);
        }

        // 2. CONTENT AREA
        ImGui::SetCursorPos(ImVec2(20, 70));
        ImGui::BeginChild("Content", ImVec2(460, 250), false);
            
            // STATUS CARD
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
            ImGui::BeginChild("StatusCard", ImVec2(0, 40), true);
                ImGui::SetCursorPos(ImVec2(10, 10));
                if (mem.processId > 0) {
                    ImGui::TextColored(ImVec4(0,1,0,1), "STATUS: CONNECTED");
                } else {
                    ImGui::TextColored(ImVec4(1,0,0,1), "STATUS: WAITING FOR GAME...");
                }
            ImGui::EndChild();
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // FEATURE TOGGLES (Using Custom Toggle)
            if (ToggleButton("  Aimbot Headshot", &bAimbot)) {
                 GuiStyles::AddToast(bAimbot ? "Aimbot Enabled" : "Aimbot Disabled", bAimbot ? 1 : 0);
            }
            ImGui::Spacing();
            if (ToggleButton("  Sniper Scope (No Recoil)", &bScope)) {
                 GuiStyles::AddToast(bScope ? "Scope Hack Enabled" : "Scope Hack Disabled", bScope ? 1 : 0);
            }
            ImGui::Spacing();
            if (ToggleButton("  Quick Switch", &bQuickSwitch)) {
                 GuiStyles::AddToast(bQuickSwitch ? "Quick Switch Enabled" : "Quick Switch Disabled", bQuickSwitch ? 1 : 0);
            }

        ImGui::EndChild();
        ImGui::End();

        // 3. TOAST NOTIFICATIONS (Overlay)
        // Draw these in a separate window on top of everything
        if (!GuiStyles::toasts.empty()) {
            ImGui::SetNextWindowPos(ImVec2(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)) - 220, static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) - 100)); // Bottom Right of SCREEN
            ImGui::SetNextWindowSize(ImVec2(200, 300)); // Tall invisible window to stack toasts
            ImGui::Begin("##Toasts", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_TopMost);
            
            float yOffset = 0.0f;
            for (auto it = GuiStyles::toasts.begin(); it != GuiStyles::toasts.end(); ) {
                ImGui::SetCursorPos(ImVec2(0, 250 - yOffset)); // Stack upwards
                ImVec4 toastColor = (it->type == 1) ? ImVec4(0, 0.8f, 0, 0.9f) : ImVec4(0.15f, 0.15f, 0.15f, 0.9f);
                
                ImGui::PushStyleColor(ImGuiCol_ChildBg, toastColor);
                ImGui::BeginChild(("Toast" + std::to_string(yOffset)).c_str(), ImVec2(180, 40), true);
                    ImGui::SetCursorPos(ImVec2(10, 10));
                    ImGui::Text("%s", it->message.c_str());
                ImGui::EndChild();
                ImGui::PopStyleColor();

                it->timer += ImGui::GetIO().DeltaTime;
                yOffset += 50.0f;

                if (it->timer > it->duration)
                    it = GuiStyles::toasts.erase(it);
                else
                    ++it;
            }
            ImGui::End();
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha); // Transparent Clear
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
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

// Custom Drag Implementation: Helper to move window
void DragWindow(HWND hwnd) {
    ReleaseCapture();
    SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

// MODERN TOGGLE WIDGET
bool ToggleButton(const char* label, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(label, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;

    float t = *v ? 1.0f : 0.0f;

    ImGuiContext& g = *GImGui;
    float ANIM_SPEED = 0.08f;
    if (g.LastActiveId == g.CurrentWindow->GetID(label)) // && g.LastActiveIdTimer < ANIM_SPEED)
        t = *v ? 1.0f : 0.0f; // Simple instant for now, can perform smooth anim with state storage

    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = ImGui::GetColorU32(*v ? ImVec4(0.0f, 0.88f, 0.0f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    else
        col_bg = ImGui::GetColorU32(*v ? ImVec4(0.0f, 0.78f, 0.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
    
    ImGui::SameLine();
    ImGui::Text("%s", label);
    return *v;
}

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
