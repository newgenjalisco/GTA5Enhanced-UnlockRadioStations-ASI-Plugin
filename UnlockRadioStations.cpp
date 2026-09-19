// Unlock Radio Stations (native ASI, no .NET) for GTA V Enhanced
// Unlocks the GTA Online public radio stations in Story Mode.
// Uses Script Hook V directly, so it does not require ScriptHookVDotNet.
//
// Method: calls AUDIO::LOCK_RADIO_STATION(stationName, FALSE) for every
// public station, which clears the "locked" state and makes the station
// appear/play in the Story Mode radio wheel.

#include <windows.h>
#include <cstdint>
#include <cstddef>

// ---- Script Hook V exports (resolved at runtime to avoid linking ScriptHookV.lib) ----
typedef void (*fn_scriptRegister)(HMODULE, void (*)());
typedef void (*fn_scriptUnregister)(HMODULE);
typedef void (*fn_scriptWait)(DWORD);
typedef void (*fn_nativeInit)(uint64_t);
typedef void (*fn_nativePush64)(uint64_t);
typedef uint64_t *(*fn_nativeCall)();

static fn_scriptRegister   p_scriptRegister   = nullptr;
static fn_scriptUnregister p_scriptUnregister = nullptr;
static fn_scriptWait       p_scriptWait       = nullptr;
static fn_nativeInit       p_nativeInit       = nullptr;
static fn_nativePush64     p_nativePush64     = nullptr;
static fn_nativeCall       p_nativeCall       = nullptr;

// AUDIO::LOCK_RADIO_STATION(const char* radioStationName, BOOL toggle)
static const uint64_t HASH_LOCK_RADIO_STATION = 0x477D9DB48F889591ULL;

// Public radio stations, by their stable internal audio names.
static const char *kStations[] = {
    "RADIO_01_CLASS_ROCK",
    "RADIO_02_POP",
    "RADIO_03_HIPHOP_NEW",
    "RADIO_04_PUNK",
    "RADIO_05_TALK_01",
    "RADIO_06_COUNTRY",
    "RADIO_07_DANCE_01",
    "RADIO_08_MEXICAN",
    "RADIO_09_HIPHOP_OLD",
    "RADIO_11_TALK_02",
    "RADIO_12_REGGAE",
    "RADIO_13_JAZZ",
    "RADIO_14_DANCE_02",
    "RADIO_15_MOTOWN",
    "RADIO_16_SILVERLAKE",
    "RADIO_17_FUNK",
    "RADIO_18_90S_ROCK",
    "RADIO_19_USER",
    "RADIO_20_THELAB",
    "RADIO_21_DLC_XM17",
    "RADIO_22_DLC_BATTLE_MIX1_RADIO",
    "RADIO_23_DLC_XM19_RADIO",
    "RADIO_27_DLC_PRHEI4",
    "RADIO_34_DLC_HEI4_KULT",
    "RADIO_35_DLC_HEI4_MLR",
    "RADIO_36_AUDIOPLAYER",
    "RADIO_37_MOTOMAMI",
};

static void lockStation(const char *name, bool lock)
{
    p_nativeInit(HASH_LOCK_RADIO_STATION);
    p_nativePush64(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(name)));
    p_nativePush64(lock ? 1ULL : 0ULL);
    p_nativeCall();
}

static void unlockAll()
{
    for (size_t i = 0; i < sizeof(kStations) / sizeof(kStations[0]); ++i)
        lockStation(kStations[i], false);
}

// Runs on the game's script thread once Script Hook V launches it.
static void ScriptMain()
{
    for (;;)
    {
        unlockAll();
        // Re-apply periodically so the stations stay unlocked after
        // save loads, cutscenes, or the game's radio script re-locking them.
        p_scriptWait(500);
    }
}

static bool resolveExports(HMODULE shv)
{
    p_scriptRegister   = reinterpret_cast<fn_scriptRegister>(
        GetProcAddress(shv, "?scriptRegister@@YAXPEAUHINSTANCE__@@P6AXXZ@Z"));
    p_scriptUnregister = reinterpret_cast<fn_scriptUnregister>(
        GetProcAddress(shv, "?scriptUnregister@@YAXPEAUHINSTANCE__@@@Z"));
    p_scriptWait       = reinterpret_cast<fn_scriptWait>(
        GetProcAddress(shv, "?scriptWait@@YAXK@Z"));
    p_nativeInit       = reinterpret_cast<fn_nativeInit>(
        GetProcAddress(shv, "?nativeInit@@YAX_K@Z"));
    p_nativePush64     = reinterpret_cast<fn_nativePush64>(
        GetProcAddress(shv, "?nativePush64@@YAX_K@Z"));
    p_nativeCall       = reinterpret_cast<fn_nativeCall>(
        GetProcAddress(shv, "?nativeCall@@YAPEA_KXZ"));

    return p_scriptRegister && p_scriptUnregister && p_scriptWait &&
           p_nativeInit && p_nativePush64 && p_nativeCall;
}

BOOL WINAPI DllMain(HMODULE hInstance, DWORD reason, LPVOID /*reserved*/)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hInstance);

        HMODULE shv = GetModuleHandleA("ScriptHookV.dll");
        if (!shv)
            shv = LoadLibraryA("ScriptHookV.dll");

        if (shv && resolveExports(shv))
            p_scriptRegister(hInstance, ScriptMain);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        if (p_scriptUnregister)
            p_scriptUnregister(hInstance);
    }
    return TRUE;
}
