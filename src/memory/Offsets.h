#pragma once
#include <vector>
#include <string>

// NAMESPACE: Offsets
// DESCRIPTION: Holds the Byte Patterns (Signatures) and the Replacement Instruction Bytes (Cheat).
// NOTE: You MUST update these patterns using Cheat Engine or from online forums (UnknownCheats) 
//       as they change with EVERY Free Fire update.

namespace Offsets {
    
    // PROCESS NAME
    // Bluestacks usually runs as "HD-Player.exe". 
    // If using MSI App Player, it might be slightly different.
    const wchar_t* GAME_PROCESS_NAME = L"HD-Player.exe";

    // PATTERN: AIMBOT
    // The sequence of bytes that identifies the Aimbot function in memory.
    // Example format: "A0 3B ?? ?? 8F" (?? is a wildcard)
    const char* AIMBOT_PATTERN = "A0 3B ?? ?? 8F ?? ?? ??"; 

    // CHEAT BYTES: AIMBOT (ENABLE)
    // The instruction you want to WRITE to enable the cheat.
    // often "NOP" (0x90) to stop the game from overwriting your angles, 
    // or a jump instruction to your own code.
    const std::vector<unsigned char> AIMBOT_ENABLE = { 0x90, 0x90, 0x90, 0x90 }; 

    // CHEAT BYTES: AIMBOT (DISABLE/RESTORE)
    // The ORIGINAL bytes that were there before you patched.
    // Crucial to prevent crashes when turning it off.
    const std::vector<unsigned char> AIMBOT_DISABLE = { 0xA0, 0x3B, 0x11, 0x22 }; // REPLACE WITH REAL BYTES

    // PATTERN: SNIPER SCOPE NO-RECOIL
    const char* SCOPE_PATTERN = "B0 4C ?? ?? 90"; 
    const std::vector<unsigned char> SCOPE_ENABLE = { 0x90, 0x90, 0x90 }; 
    const std::vector<unsigned char> SCOPE_DISABLE = { 0xB0, 0x4C, 0x01 };

    // PATTERN: SNIPER QUICK SWITCH
    const char* QUICKSWITCH_PATTERN = "C0 5D ?? ?? 88";
    const std::vector<unsigned char> QUICKSWITCH_ENABLE = { 0x90, 0x90 };
    const std::vector<unsigned char> QUICKSWITCH_DISABLE = { 0xC0, 0x5D };

}
