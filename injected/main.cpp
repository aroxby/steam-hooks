#include <iostream>
#include <windows.h>

#include "../mini-steam/steam_api.h"
#include "MinHook.h"

using namespace std;

typedef ISteamUserStats *(*pfnSteamUserStats)();
typedef __thiscall bool (*pfnSetAchievement)(ISteamUserStats *self, const char *pchName);

pfnSteamUserStats pOriginalSteamUserStats = nullptr;
pfnSetAchievement pOriginalSetAchievement = nullptr;

__thiscall bool hookedSetAchievement(ISteamUserStats *self, const char *pchName) {
    cout << "Hooked SetAchievement called for: " << pchName << endl;
    return pOriginalSetAchievement(self, pchName);
}

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

bool hookSetAchievement(ISteamUserStats *stats) {
    pfnSetAchievement setAchievement = getSetAchievementFunc(stats);

    if (MH_CreateHook((LPVOID)setAchievement, (LPVOID)&hookedSetAchievement, (LPVOID *)&pOriginalSetAchievement) !=
        MH_OK) {
        cerr << "Failed to create hook for SetAchievement!" << std::endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)setAchievement) != MH_OK) {
        cerr << "Failed to enable hook for SetAchievement!" << std::endl;
        return false;
    }
    return true;
}

ISteamUserStats *steamUserStatHook() {
    auto stats = pOriginalSteamUserStats();
    if (pOriginalSetAchievement) {
        return stats; // ALready hooked
    }

    if (!stats) {
        return stats;
    }

    hookSetAchievement(stats);

    return stats;
}

bool hookSteamUserStats() {
    HMODULE hSteamAPI = LoadLibraryA("steam_api.dll");
    if (!hSteamAPI) {
        cerr << "Failed to load steam_api.dll!" << std::endl;
        return false;
    }

    pfnSteamUserStats steamUserStats = (pfnSteamUserStats)GetProcAddress(hSteamAPI, "SteamUserStats");
    if (!steamUserStats) { // TODO: This is okay if SteamAPI_ISteamUserStats_SetAchievement is present
        cerr << "Failed to get SteamUserStats function!" << std::endl;
        return false;
    }

    if (MH_CreateHook((LPVOID)steamUserStats, (LPVOID)&steamUserStatHook, (LPVOID *)&pOriginalSteamUserStats) !=
        MH_OK) {
        cerr << "Failed to create hook for SteamUserStats!" << std::endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)steamUserStats) != MH_OK) {
        cerr << "Failed to enable hook for SteamUserStats!" << std::endl;
        return false;
    }

    return true;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        if (MH_Initialize() != MH_OK) {
            cerr << "Failed to initialize MinHook." << std::endl;
            return FALSE;
        }

        hookSteamUserStats();
        break;

    case DLL_PROCESS_DETACH:
        // MH_DisableHook((LPVOID)&SteamUserStats);
        // MH_Uninitialize();
        break;
    }
    return TRUE;
}
