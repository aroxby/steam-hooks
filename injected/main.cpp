#include <iostream>
#include <windows.h>

#include "MinHook.h"

using namespace std;

typedef int(WINAPI *MESSAGEBOXA)(HWND, LPCSTR, LPCSTR, UINT);
MESSAGEBOXA OriginalMessageBoxA = nullptr;

int WINAPI DetourMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    return OriginalMessageBoxA(hWnd, "DLL Injected!", lpCaption, uType);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        if (MH_Initialize() != MH_OK) {
            cerr << "Failed to initialize MinHook." << std::endl;
            return FALSE;
        }

        if (MH_CreateHook((LPVOID)&MessageBoxA, (LPVOID)&DetourMessageBoxA, (LPVOID *)&OriginalMessageBoxA) != MH_OK) {
            cerr << "Failed to create hook." << std::endl;
            MH_Uninitialize();
            return FALSE;
        }

        if (MH_EnableHook((LPVOID)&MessageBoxA) != MH_OK) {
            cerr << "Failed to enable hook." << std::endl;
            MH_Uninitialize();
            return FALSE;
        }
        break;

    case DLL_PROCESS_DETACH:
        MH_DisableHook((LPVOID)&MessageBoxA);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}
