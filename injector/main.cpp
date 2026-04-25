#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#include "cmdline.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: steam-hooks <program> [args...]\n";
        return 1;
    }

    wstring commandLine = BuildCommandLine(argc, argv);
    vector<wchar_t> mutableCommand(commandLine.begin(), commandLine.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};
    BOOL ok =
        CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr, FALSE,
                       0, nullptr, nullptr, &startupInfo, &processInfo);

    if (!ok) {
        DWORD err = GetLastError();
        cerr << "CreateProcessW failed with error " << err << "\n";
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
