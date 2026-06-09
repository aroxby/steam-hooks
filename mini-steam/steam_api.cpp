#include <iostream>

#include "steam_api.h"

using namespace std;

#define LOG_CALL() (std::cout << "mini-steam stub (" << __func__ << ")" << std::endl);

class CSteamUserStats : public ISteamUserStats {
  public:
    virtual bool RequestCurrentStats() {
        LOG_CALL();
        return false;
    }

    virtual bool GetStatInt32(const char *pchName, int32_t *pData) {
        LOG_CALL();
        return false;
    }
    virtual bool GetStatFloat(const char *pchName, float *pData) {
        LOG_CALL();
        return false;
    }
    virtual bool SetStatInt32(const char *pchName, int32_t nData) {
        LOG_CALL();
        return false;
    }
    virtual bool SetStatFloat(const char *pchName, float fData) {
        LOG_CALL();
        return false;
    }

    virtual bool UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength) {
        LOG_CALL();
        return false;
    }

    virtual bool GetAchievement(const char *pchName, bool *pbAchieved) {
        LOG_CALL();
        return false;
    }
    virtual bool SetAchievement(const char *pchName) {
        LOG_CALL();
        return false;
    }
    virtual bool ClearAchievement(const char *pchName) {
        LOG_CALL();
        return false;
    }

    virtual bool GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, int32_t *punUnlockTime) {
        LOG_CALL();
        return false;
    }
    virtual bool StoreStats() {
        LOG_CALL();
        return false;
    }
    virtual int GetAchievementIcon(const char *pchName) {
        LOG_CALL();
        return 0;
    }
    virtual const char *GetAchievementDisplayAttribute(const char *pchName, const char *pchKey) {
        LOG_CALL();
        return nullptr;
    }
};

CSteamUserStats g_SteamUserStats;

S_API ISteamUserStats *SteamUserStats() { return &g_SteamUserStats; }
