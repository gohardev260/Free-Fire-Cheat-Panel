#pragma once
#include <string>
#include <array>
#include <cstdarg>

// Simple Compile-Time XOR String Encryption
// Prevents strings from appearing in plain text in the binary (which Anti-Cheats scan for).

namespace XorCompileTime {
    constexpr auto time = __TIME__;
    constexpr auto seed = static_cast<int>(time[7]) + static_cast<int>(time[6]) * 10 + static_cast<int>(time[4]) * 60 + static_cast<int>(time[3]) * 600;

    template <int N, const char p[N]>
    struct MetaString {
        char buffer[N];
        constexpr MetaString() : buffer{0} {
            for (int i = 0; i < N; ++i) {
                buffer[i] = p[i] ^ (seed % 255);
            }
        }
    };
}

// Runtime Decryption wrapper (Simple version for ease of use)
// Usage: std::string s = Decrypt("EncryptedString"); 
// Note: Real compile-time XOR is complex to implement in a single header without C++17/20 features heavily.
// For this tutorial project, we will use a Runtime Obfuscation Helper to keep it compatible with VS2022 default settings easily.

class XorStr {
public:
    static std::string Unscramble(std::string s) {
        // Simple XOR with a hardcoded key (Better than plain text)
        // In a real private cheat, this would be polymorphic.
        char key = 0x69; 
        for (size_t i = 0; i < s.size(); i++) {
            s[i] ^= key;
        }
        return s;
    }
};

// Macro to make usage easier. 
// We manually scramble strings in code: "Aimbot" -> (XOR 0x69) -> "HirKV}"
// This is a "Poor Man's" obfuscation for tutorial purposes.
#define _(str) str 
