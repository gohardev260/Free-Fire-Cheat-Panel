#include "Memory.h"

bool Memory::Attach(const wchar_t* processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName) == 0) {
                processId = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);

    if (processId != 0) {
        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        return (processHandle != NULL);
    }

    return false;
}

void Memory::Detach() {
    if (processHandle) {
        CloseHandle(processHandle);
    }
}

bool Memory::Patch(uintptr_t address, const std::vector<unsigned char>& bytes) {
    if (!processHandle) return false;

    // Change protection to ExecuteReadWrite to allow patching
    DWORD oldProtect;
    VirtualProtectEx(processHandle, (LPVOID)address, bytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

    SIZE_T bytesWritten;
    BOOL result = WriteProcessMemory(processHandle, (LPVOID)address, bytes.data(), bytes.size(), &bytesWritten);

    // Restore original protection
    VirtualProtectEx(processHandle, (LPVOID)address, bytes.size(), oldProtect, &oldProtect);

    return result != 0;
}

uintptr_t Memory::GetModuleBaseAddress(const wchar_t* moduleName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (_wcsicmp(modEntry.szModule, moduleName) == 0) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}
