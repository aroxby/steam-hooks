#pragma once

#include <windows.h>

// Resolves the path to injected.dll relative to the injector executable.
// Assumes injected.dll is located at ../injected/injected.dll relative to the injector.
// Returns true if the DLL file exists at the resolved path, false otherwise.
// If successful, outPath is filled with the full path to injected.dll.
bool ResolveInjectedDllPath(wchar_t *outPath, size_t outPathSize);
