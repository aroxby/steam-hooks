#pragma once

#include <cstdint>

// S_API defines the linkage and calling conventions for steam_api.dll exports
#if defined(_WIN32) && !defined(_X360)
#if defined(STEAM_API_EXPORTS)
#define S_API extern "C" __declspec(dllexport)
#elif defined(STEAM_API_NODLL)
#define S_API extern "C"
#else
#define S_API extern "C" __declspec(dllimport)
#endif // STEAM_API_EXPORTS
#elif defined(__GNUC__)
#if defined(STEAM_API_EXPORTS)
#define S_API extern "C" __attribute__((visibility("default")))
#else
#define S_API extern "C"
#endif // STEAM_API_EXPORTS
#else  // !WIN32
#if defined(STEAM_API_EXPORTS)
#define S_API extern "C"
#else
#define S_API extern "C"
#endif // STEAM_API_EXPORTS
#endif

class ISteamUserStats {
  public:
    virtual bool RequestCurrentStats() = 0;

    virtual bool GetStatInt32(const char *pchName, int32_t *pData) = 0;
    virtual bool GetStatFloat(const char *pchName, float *pData) = 0;
    virtual bool SetStatInt32(const char *pchName, int32_t nData) = 0;
    virtual bool SetStatFloat(const char *pchName, float fData) = 0;

    virtual bool UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength) = 0;

    virtual bool GetAchievement(const char *pchName, bool *pbAchieved) = 0;
    virtual bool SetAchievement(const char *pchName) = 0;
    virtual bool ClearAchievement(const char *pchName) = 0;

    virtual bool GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, int32_t *punUnlockTime) = 0;
    virtual bool StoreStats() = 0;
    virtual int GetAchievementIcon(const char *pchName) = 0;
    virtual const char *GetAchievementDisplayAttribute(const char *pchName, const char *pchKey) = 0;

    // Additional methods of ISteamUserStats have been omitted for brevity.
};

S_API ISteamUserStats *SteamUserStats();
