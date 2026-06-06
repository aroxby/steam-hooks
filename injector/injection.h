#pragma once

#include <windows.h>

// Injects a DLL into the target process using CreateRemoteThread + LoadLibraryW.
// The process must be in a valid state (typically created with CREATE_SUSPENDED).
// dllPath should be a full path to the DLL file.
// Returns true if injection succeeded, false otherwise.
bool InjectDLL(PROCESS_INFORMATION pi, const wchar_t *dllPath);
