#pragma once
#include <string>
#include <random>
#include <algorithm>
#include <Windows.h>

namespace Security {

    std::string RandomString(size_t length) {
        const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, (int)characters.size() - 1);

        std::string random_string;
        for (size_t i = 0; i < length; ++i) {
            random_string += characters[distribution(generator)];
        }
        return random_string;
    }

    void InitAntiDetect() {
        // 1. Randomize Window Title
        // Instead of "Gohar Xiters", we set it to something random like "x8ds9f0"
        // This stops Anti-Cheats from finding the window by Name.
        std::string randomTitle = RandomString(12);
        SetConsoleTitleA(randomTitle.c_str());
        
        // Note: For our GUI Window, we handle this in WinMain.
    }
}
