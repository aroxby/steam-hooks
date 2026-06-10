#include <iostream>
#include <windows.h>

#include "../mini-steam/steam_api.h"
#include "MinHook.h"

using namespace std;

typedef ISteamUserStats *(*pfnSteamUserStats)();
typedef __thiscall bool (*pfnGetAchievement)(ISteamUserStats *self, const char *pchName, bool *pbAchieved);
typedef __thiscall bool (*pfnSetAchievement)(ISteamUserStats *self, const char *pchName);
typedef void *(*pfnFindOrCreateUserInterface)(int32_t *hSteamUser, const char *pszVersion);

typedef ISteamUtils *(*pfnSteamUtils)();

pfnSteamUserStats pOriginalSteamUserStats = nullptr;
pfnGetAchievement pOriginalGetAchievement = nullptr;
pfnSetAchievement pOriginalSetAchievement = nullptr;
pfnFindOrCreateUserInterface pOriginalFindOrCreateUserInterface = nullptr;

pfnSteamUtils pOriginalSteamUtils = nullptr;

__thiscall bool hookedSetAchievement(ISteamUserStats *self, const char *pchName) {
    cout << "Hooked SetAchievement called for: " << pchName << endl;
    self->ClearAchievement(pchName);
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
        cerr << "Failed to create hook for SetAchievement!" << endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)setAchievement) != MH_OK) {
        cerr << "Failed to enable hook for SetAchievement!" << endl;
        return false;
    }
    return true;
}

__thiscall bool hookedGetAchievement(ISteamUserStats *self, const char *pchName, bool *pbAchieved) {
    cout << "Hooked GetAchievement called for: " << pchName << endl;
    bool result = pOriginalGetAchievement(self, pchName, pbAchieved);
    if (pbAchieved) {
        *pbAchieved = false;
    }
    return result;
}

pfnGetAchievement getGetAchievementFunc(ISteamUserStats *stats) {
    void **vtable = *(void ***)stats;
    auto getAchievement = ISteamUserStats::GetAchievement;
    size_t index = (size_t)(void *)getAchievement / sizeof(void *);
    cout << "GetAchievement vtable index: " << index << endl;
    return (pfnGetAchievement)vtable[index];
}

bool hookGetAchievement(ISteamUserStats *stats) {
    pfnGetAchievement getAchievement = getGetAchievementFunc(stats);

    if (MH_CreateHook((LPVOID)getAchievement, (LPVOID)&hookedGetAchievement, (LPVOID *)&pOriginalGetAchievement) !=
        MH_OK) {
        cerr << "Failed to create hook for GetAchievement!" << endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)getAchievement) != MH_OK) {
        cerr << "Failed to enable hook for GetAchievement!" << endl;
        return false;
    }
    return true;
}

ISteamUserStats *hookedSteamUserStats() {
    auto stats = pOriginalSteamUserStats();
    if (pOriginalSetAchievement) {
        return stats; // ALready hooked
    }

    if (!stats) {
        return stats;
    }

    hookGetAchievement(stats);
    hookSetAchievement(stats);

    return stats;
}

bool hookSteamUserStats() {
    HMODULE hSteamAPI;
    (hSteamAPI = LoadLibraryA("steam_api.dll")) || (hSteamAPI = LoadLibraryA("bin/steam_api.dll"));
    if (!hSteamAPI) {
        cerr << "Failed to load steam_api.dll!" << endl;
        return false;
    }

    pfnSteamUserStats steamUserStats = (pfnSteamUserStats)GetProcAddress(hSteamAPI, "SteamUserStats");
    if (!steamUserStats) {
        cerr << "Failed to get SteamUserStats function!" << endl;
        return false;
    }

    if (MH_CreateHook((LPVOID)steamUserStats, (LPVOID)&hookedSteamUserStats, (LPVOID *)&pOriginalSteamUserStats) !=
        MH_OK) {
        cerr << "Failed to create hook for SteamUserStats!" << endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)steamUserStats) != MH_OK) {
        cerr << "Failed to enable hook for SteamUserStats!" << endl;
        return false;
    }

    return true;
}

void *hookedFindOrCreateUserInterface(int32_t *hSteamUser, const char *pszVersion) {
    // cout << "FindOrCreateUserInterface called: " << pszVersion << endl;
    auto hInterface = pOriginalFindOrCreateUserInterface(hSteamUser, pszVersion);
    if (hInterface && strstr(pszVersion, "STEAMUSERSTATS_INTERFACE_VERSION") && !pOriginalSetAchievement) {
        cout << "Found SteamUserStats interface." << endl;
        hookGetAchievement((ISteamUserStats *)hInterface);
        hookSetAchievement((ISteamUserStats *)hInterface);
    }
    if (hInterface && strstr(pszVersion, "SteamUtils") && !pOriginalSteamUtils) {
        cout << "Found SteamUtils interface." << endl;
        ISteamUtils *utils = (ISteamUtils *)hInterface;
        utils->SetOverlayNotificationPosition(ENotificationPosition::k_EPositionTopLeft);
    }
    return hInterface;
}

bool hookFindOrCreateUserInterface() {
    HMODULE hSteamAPI;
    (hSteamAPI = LoadLibraryA("steam_api.dll")) || (hSteamAPI = LoadLibraryA("bin/steam_api.dll"));
    if (!hSteamAPI) {
        cerr << "Failed to load steam_api.dll!" << endl;
        return false;
    }

    pfnFindOrCreateUserInterface findOrCreateUserInterface =
        (pfnFindOrCreateUserInterface)GetProcAddress(hSteamAPI, "SteamInternal_FindOrCreateUserInterface");
    if (!findOrCreateUserInterface) {
        cerr << "Failed to get SteamInternal_FindOrCreateUserInterface function!" << endl;
        return false;
    }

    if (MH_CreateHook((LPVOID)findOrCreateUserInterface, (LPVOID)&hookedFindOrCreateUserInterface,
                      (LPVOID *)&pOriginalFindOrCreateUserInterface) != MH_OK) {
        cerr << "Failed to create hook for SteamInternal_FindOrCreateUserInterface!" << endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)findOrCreateUserInterface) != MH_OK) {
        cerr << "Failed to enable hook for SteamInternal_FindOrCreateUserInterface!" << endl;
        return false;
    }

    return true;
}

ISteamUtils *hookedSteamUtils() {
    auto utils = pOriginalSteamUtils();
    utils->SetOverlayNotificationPosition(ENotificationPosition::k_EPositionTopLeft);
    return utils;
}

bool hookSteamUtils() {
    HMODULE hSteamAPI;
    (hSteamAPI = LoadLibraryA("steam_api.dll")) || (hSteamAPI = LoadLibraryA("bin/steam_api.dll"));
    if (!hSteamAPI) {
        cerr << "Failed to load steam_api.dll!" << endl;
        return false;
    }

    pfnSteamUtils steamUtils = (pfnSteamUtils)GetProcAddress(hSteamAPI, "SteamUtils");
    if (!steamUtils) {
        cerr << "Failed to get SteamUtils function!" << endl;
        return false;
    }

    if (MH_CreateHook((LPVOID)steamUtils, (LPVOID)&hookedSteamUtils, (LPVOID *)&pOriginalSteamUtils) != MH_OK) {
        cerr << "Failed to create hook for SteamUtils!" << endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)steamUtils) != MH_OK) {
        cerr << "Failed to enable hook for SteamUtils!" << endl;
        return false;
    }

    return true;
}

void attachConsole() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }

    if (!GetConsoleWindow()) {
        return;
    }

    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    std::ios_base::sync_with_stdio();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        attachConsole();

        if (MH_Initialize() != MH_OK) {
            cerr << "Failed to initialize MinHook." << endl;
            return FALSE;
        }

        hookSteamUserStats();
        hookFindOrCreateUserInterface();
        hookSteamUtils();
        break;

    case DLL_PROCESS_DETACH:
        // MH_DisableHook((LPVOID)&SteamUserStats);
        // MH_Uninitialize();
        break;
    }
    return TRUE;
}
