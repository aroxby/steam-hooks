#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#include "cmdline.h"
#include "injection.h"
#include "path_utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: steam-hooks <program> [args...]\n";
        return 1;
    }

    // Resolve the path to injected.dll
    wchar_t dllPath[MAX_PATH];
    if (!ResolveInjectedDllPath(dllPath, MAX_PATH)) {
        cerr << "Failed to resolve injected.dll path. Make sure it exists at ../injected/injected.dll\n";
        return 1;
    }

    wstring commandLine = BuildCommandLine(argc, argv);
    vector<wchar_t> mutableCommand(commandLine.begin(), commandLine.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};
    // Create process in suspended state so we can inject the DLL before it runs
    BOOL ok = CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr,
                             nullptr, &startupInfo, &processInfo);

    if (!ok) {
        DWORD err = GetLastError();
        cerr << "CreateProcessW failed with error " << err << "\n";
        return static_cast<int>(err);
    }

    // Inject the DLL into the suspended process
    if (!InjectDLL(processInfo, dllPath)) {
        cerr << "DLL injection failed\n";
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 1;
    }

    // Resume the process now that the DLL is injected
    if (ResumeThread(processInfo.hThread) == (DWORD)-1) {
        DWORD err = GetLastError();
        cerr << "ResumeThread failed with error " << err << "\n";
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return static_cast<int>(err);
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    DWORD childExitCode = 0;
    if (!GetExitCodeProcess(processInfo.hProcess, &childExitCode)) {
        DWORD err = GetLastError();
        cerr << "GetExitCodeProcess failed with error " << err << "\n";
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return static_cast<int>(err);
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return static_cast<int>(childExitCode);
}
