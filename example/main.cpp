#include <windows.h>

int main(int argc, char *argv[]) {
    MessageBoxA(nullptr, "Startup", "Example", MB_OK);
    MessageBoxA(nullptr, "Shutdown", "Example", MB_OK);
    return 0;
}
