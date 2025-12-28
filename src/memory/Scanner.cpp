#include "Scanner.h"
#include <vector>

// Internal helper to compare bytes
bool CompareBytes(const unsigned char* data, const char* pattern, const char* mask) {
    for (; *mask; ++mask, ++pattern, ++data) {
        if (*mask == 'x' && *data != (unsigned char)*pattern) {
            return false;
        }
    }
    return true;
}

uintptr_t Scanner::FindPattern(Memory& mem, uintptr_t startAddress, uintptr_t endAddress, const char* pattern, const char* mask) {
    // Determine the size of the memory to read
    size_t size = endAddress - startAddress;
    if (size == 0) return 0;

    // Read the entire memory block into a local buffer
    // NOTE: For large games, reading the WHOLE module at once is memory intensive.
    // Better approaches use chunks. For this demo, we read in 4096 byte chunks.
    
    const size_t CHUNK_SIZE = 4096;
    std::vector<unsigned char> buffer(CHUNK_SIZE);
    size_t patternLength = strlen(mask);

    for (uintptr_t i = startAddress; i < endAddress; i += CHUNK_SIZE) {
        // Handle last chunk being smaller
        size_t readSize = (i + CHUNK_SIZE > endAddress) ? (endAddress - i) : CHUNK_SIZE;
        
        // Read Memory
        SIZE_T bytesRead;
        ReadProcessMemory(mem.processHandle, (LPCVOID)i, buffer.data(), readSize, &bytesRead);
        
        if (bytesRead == 0) continue;

        // Scan this chunk
        for (size_t b = 0; b < bytesRead - patternLength; ++b) {
            if (CompareBytes(buffer.data() + b, pattern, mask)) {
                return i + b;
            }
        }
    }

    return 0;
}
