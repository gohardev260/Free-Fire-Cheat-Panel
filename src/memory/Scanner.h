#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include "Memory.h"

class Scanner {
public:
    // Pattern Scan: Search for "A0 ?? 8B" style patterns
    // Returns the address where the pattern was found, or 0 if not found.
    static uintptr_t FindPattern(Memory& mem, uintptr_t startAddress, uintptr_t endAddress, const char* pattern, const char* mask);
    
    // Helper to parse IDA style strings "A0 90 ? CC" directly if needed
    // (Simplified version provided here uses Mask + Char array)
};
