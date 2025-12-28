#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

namespace AntiCrack {

    // 1. Anti-Debug: Check if a debugger is attached
    bool IsDebugged() {
        // Basic check
        if (IsDebuggerPresent()) return true;

        // Remote check (for detached debuggers)
        BOOL isRemote = FALSE;
        CheckRemoteDebuggerPresent(GetCurrentProcess(), &isRemote);
        if (isRemote) return true;

        return false;
    }

    // 2. Anti-Dump: Erase PE Headers from memory
    // This makes it hard for tools to "dump" the EXE from RAM back to Disk.
    void ErasePE() {
        // Get base address of this module
        HMODULE hModule = GetModuleHandle(NULL);
        if (!hModule) return;

        // The PE Header starts at the base address
        DWORD_PTR baseAddr = (DWORD_PTR)hModule;

        // Protection allows us to WRITE to the header memory
        DWORD oldProtect;
        if (VirtualProtect((LPVOID)baseAddr, 4096, PAGE_READWRITE, &oldProtect)) {
            // Overwrite the first 4096 bytes (DOS Header + NT Header + Section Headers)
            // with Zeros.
            ZeroMemory((LPVOID)baseAddr, 4096);
            
            // Restore protection (optional, keeping it RW might confuse some tools further)
            // VirtualProtect((LPVOID)baseAddr, 4096, oldProtect, &oldProtect);
        }
    }

    // Main Protection Function to call at startup
    void RunChecks() {
        if (IsDebugged()) {
            // Crash or Exit
            exit(0);
        }
        
        // Unsafe to do this immediately if other libs need the header, 
        // but typically safe after initial CRT startup.
        ErasePE(); 
    }
}
