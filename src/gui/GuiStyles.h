#pragma once
#include <string>
#include <vector>
#include <map>
// #include "imgui.h" // User must include this
// #include "imgui_internal.h" // For animations

// Forward declaration if ImGui is not yet linked
struct ImVec4;

namespace GuiStyles {

    // --- COLORS ---
    // Hacker Theme Palette
    const auto COL_BG = 0xFF121212; // Dark Grey/Black
    const auto COL_ACCENT = 0xFF00FF00; // Bright Green
    const auto COL_TEXT = 0xFFFFFFFF; // White
    const auto COL_TEXT_DIM = 0xFFAAAAAA; // Grey

    // --- ANIMATION STATE ---
    // Holds the 0.0f - 1.0f value for toggle animations
    static std::map<std::string, float> animState;

    // --- TOAST NOTIFICATIONS ---
    struct Toast {
        std::string message;
        float timer; // 0.0f to duration
        float duration;
        int type; // 0=Info, 1=Success, 2=Warning
    };
    static std::vector<Toast> toasts;

    void AddToast(const std::string& msg, int type = 0) {
        toasts.push_back({ msg, 0.0f, 3.0f, type });
    }

    // --- THEME SETUP ---
    // Call this after ImGui::CreateContext()
    // Note: Since I cannot include imgui.h directly here without compilation errors 
    // (as users don't have it yet), I am writing the LOGIC for them to paste or use.
    
    /* 
    void SetupTheme(ImGuiStyle& style) {
        style.WindowRounding = 10.0f;
        style.FrameRounding = 5.0f;
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.95f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Hacker Green Text
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    } 
    */

    /*
    // START EXAMPLE ANIMATED TOGGLE
    bool AnimatedToggle(const char* label, bool* v) {
        // Custom ImGui Widget logic would go here using ImGui::InvisibleButton
        // and ImGui::GetWindowDrawList()->AddRectFilled...
        return ImGui::Checkbox(label, v); // Fallback for now
    }
    */
}
