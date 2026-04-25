#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL, "Hello from the injected DLL!", "DLL Injection", MB_OK);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
