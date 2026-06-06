#include "path_utils.h"
#include <cstring>

bool ResolveInjectedDllPath(wchar_t *outPath, size_t outPathSize) {
    // Get the full path to the injector executable
    wchar_t injectorPath[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, injectorPath, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        return false;
    }

    // Find the last backslash to isolate the directory
    wchar_t *lastBackslash = wcsrchr(injectorPath, L'\\');
    if (lastBackslash == nullptr) {
        return false;
    }

    // Find the parent directory by going up one level
    wchar_t parentPath[MAX_PATH];
    wcsncpy_s(parentPath, MAX_PATH, injectorPath, lastBackslash - injectorPath);

    // Find the last backslash in parentPath to isolate its parent
    wchar_t *lastBackslashParent = wcsrchr(parentPath, L'\\');
    if (lastBackslashParent == nullptr) {
        return false;
    }

    // Build the full path to injected.dll: ../injected/injected.dll
    size_t parentLen = lastBackslashParent - parentPath + 1; // include the backslash
    if (parentLen + wcslen(L"injected\\injected.dll") >= outPathSize) {
        return false; // Path too long
    }

    wcsncpy_s(outPath, outPathSize, parentPath, parentLen);
    wcscat_s(outPath, outPathSize, L"injected\\injected.dll");

    // Check if the file exists
    DWORD attributes = GetFileAttributesW(outPath);
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}
