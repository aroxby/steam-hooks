#include "injection.h"
#include <iostream>

using namespace std;

bool InjectDLL(PROCESS_INFORMATION pi, const wchar_t *dllPath) {
    // Get the address of LoadLibraryW in the target process
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32 == nullptr) {
        cerr << "Failed to get kernel32.dll handle\n";
        return false;
    }

    FARPROC loadLibraryW = GetProcAddress(kernel32, "LoadLibraryW");
    if (loadLibraryW == nullptr) {
        cerr << "Failed to get LoadLibraryW address\n";
        return false;
    }

    // Allocate memory in the target process for the DLL path string
    size_t dllPathLen = wcslen(dllPath);
    size_t allocSize = (dllPathLen + 1) * sizeof(wchar_t);

    LPVOID remoteBuffer = VirtualAllocEx(pi.hProcess, nullptr, allocSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteBuffer == nullptr) {
        cerr << "VirtualAllocEx failed with error " << GetLastError() << "\n";
        return false;
    }

    // Write the DLL path into the allocated memory
    if (!WriteProcessMemory(pi.hProcess, remoteBuffer, dllPath, allocSize, nullptr)) {
        cerr << "WriteProcessMemory failed with error " << GetLastError() << "\n";
        VirtualFreeEx(pi.hProcess, remoteBuffer, 0, MEM_RELEASE);
        return false;
    }

    // Create a remote thread to call LoadLibraryW with the DLL path
    HANDLE remoteThread =
        CreateRemoteThread(pi.hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLibraryW, remoteBuffer, 0, nullptr);
    if (remoteThread == nullptr) {
        cerr << "CreateRemoteThread failed with error " << GetLastError() << "\n";
        VirtualFreeEx(pi.hProcess, remoteBuffer, 0, MEM_RELEASE);
        return false;
    }

    // Wait for the remote thread to complete
    DWORD waitResult = WaitForSingleObject(remoteThread, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        cerr << "WaitForSingleObject failed with error " << GetLastError() << "\n";
        CloseHandle(remoteThread);
        VirtualFreeEx(pi.hProcess, remoteBuffer, 0, MEM_RELEASE);
        return false;
    }

    // Get the exit code of the remote thread (HMODULE from LoadLibraryW)
    DWORD moduleHandle = 0;
    if (!GetExitCodeThread(remoteThread, &moduleHandle)) {
        cerr << "GetExitCodeThread failed with error " << GetLastError() << "\n";
        CloseHandle(remoteThread);
        VirtualFreeEx(pi.hProcess, remoteBuffer, 0, MEM_RELEASE);
        return false;
    }

    // Check if LoadLibraryW succeeded (non-zero HMODULE)
    bool success = (moduleHandle != 0);
    if (!success) {
        cerr << "LoadLibraryW returned NULL in target process\n";
    }

    // Clean up
    CloseHandle(remoteThread);
    VirtualFreeEx(pi.hProcess, remoteBuffer, 0, MEM_RELEASE);

    return success;
}
