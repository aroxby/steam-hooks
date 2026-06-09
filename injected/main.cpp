#include <iostream>
#include <windows.h>

#include "../mini-steam/steam_api.h"
#include "MinHook.h"

using namespace std;

typedef ISteamUserStats *(*pfnSteamUserStats)();
typedef __thiscall bool (*pfnSetAchievement)(ISteamUserStats *self, const char *pchName);

pfnSteamUserStats pOriginalSteamUserStats = nullptr;
pfnSetAchievement pOriginalSetAchievement = nullptr;

pfnSetAchievement getSetAchievementFunc(ISteamUserStats *stats) {
    void **vtable = *(void ***)stats;
    auto setAchievement = ISteamUserStats::SetAchievement;
    // Note the actual value of setAchievement here is something like 57
    // The lowest bit indicates a virtual function, so the correct resolution is closer to
    // setAchievement & ~1 / 8 but integer division rounds off the odd bit
    size_t index = (size_t)(void *)setAchievement / sizeof(void *);
    cout << "SetAchievement vtable index: " << index << endl;
    return (pfnSetAchievement)vtable[index];
}

__thiscall bool HookedSetAchievement(ISteamUserStats *self, const char *pchName) {
    cout << "Hooked SetAchievement called for: " << pchName << endl;
    return pOriginalSetAchievement(self, pchName);
}

ISteamUserStats *SteamUserStatHook() {
    auto stats = pOriginalSteamUserStats();
    if (pOriginalSetAchievement) {
        return stats; // ALready hooked
    }

    if (!stats) {
        return stats;
    }

    pfnSetAchievement setAchievement = getSetAchievementFunc(stats);

    if (MH_CreateHook((LPVOID)setAchievement, (LPVOID)&HookedSetAchievement, (LPVOID *)&pOriginalSetAchievement) !=
        MH_OK) {
        cerr << "Failed to create hook for SetAchievement!" << std::endl;
        return stats;
    }

    if (MH_EnableHook((LPVOID)setAchievement) != MH_OK) {
        cerr << "Failed to enable hook for SetAchievement!" << std::endl;
        return stats;
    }

    return stats;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    HMODULE hSteamAPI = nullptr;
    pfnSteamUserStats steamUserStats = nullptr;

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        hSteamAPI = LoadLibraryA("steam_api.dll");
        if (!hSteamAPI) {
            cerr << "Failed to load steam_api.dll!" << std::endl;
            return FALSE;
        }

        steamUserStats = (pfnSteamUserStats)GetProcAddress(hSteamAPI, "SteamUserStats");
        if (!pOriginalSteamUserStats) { // TODO: This is okay if SteamAPI_ISteamUserStats_SetAchievement is present
            cerr << "Failed to get SteamUserStats function!" << std::endl;
            return FALSE;
        }

        if (MH_Initialize() != MH_OK) {
            cerr << "Failed to initialize MinHook." << std::endl;
            return FALSE;
        }

        if (MH_CreateHook((LPVOID)steamUserStats, (LPVOID)&SteamUserStatHook, (LPVOID *)&pOriginalSteamUserStats) !=
            MH_OK) {
            cerr << "Failed to create hook for SteamUserStats!" << std::endl;
            MH_Uninitialize();
            return FALSE;
        }

        if (MH_EnableHook((LPVOID)&steamUserStats) != MH_OK) {
            cerr << "Failed to enable hook for SteamUserStats!" << std::endl;
            MH_Uninitialize();
            return FALSE;
        }
        break;

    case DLL_PROCESS_DETACH:
        // MH_DisableHook((LPVOID)&SteamUserStats);
        // MH_Uninitialize();
        break;
    }
    return TRUE;
}
