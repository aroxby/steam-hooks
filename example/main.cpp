#include <windows.h>

#include "../mini-steam/steam_api.h"

int main(int argc, char *argv[]) {
    ISteamUserStats *pUserStats = SteamUserStats();
    pUserStats->SetAchievement("example");
    return 0;
}
