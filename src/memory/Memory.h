#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>
#include <iostream>

class Memory {
public:
    DWORD processId = 0;
    HANDLE processHandle = NULL;

    // Initialize: Finds the process and opens a handle
    bool Attach(const wchar_t* processName);

    // Close handle when done
    void Detach();

    // Read Memory (Template for any type)
    template <typename T>
    T Read(uintptr_t address) {
        T value;
        ReadProcessMemory(processHandle, (LPCVOID)address, &value, sizeof(T), NULL);
        return value;
    }

    // Write Memory (Template for any type)
    template <typename T>
    void Write(uintptr_t address, T value) {
        WriteProcessMemory(processHandle, (LPVOID)address, &value, sizeof(T), NULL);
    }

    // Patch: Write a vector of bytes (Shellcode) to an address
    bool Patch(uintptr_t address, const std::vector<unsigned char>& bytes);

    // Get Module Base Address (needed for scanning relative to modules like "libil2cpp.so" if emulated)
    uintptr_t GetModuleBaseAddress(const wchar_t* moduleName);
};
