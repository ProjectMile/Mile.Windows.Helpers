/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.cpp
 * PURPOSE:   Implementation for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "Mile.Helpers.h"

namespace
{
    static bool IsPrivatePerMonitorSupportExtensionApplicable()
    {
        static bool CachedResult = ([]() -> bool
        {
            // The private Per-Monitor DPI Awareness support extension is
            // Windows 10 only.
            if (!::MileIsWindowsVersionAtLeast(10, 0, 0))
            {
                return false;
            }

            // We don't need the private Per-Monitor DPI Awareness support
            // extension if the Per-Monitor (V2) DPI Awareness exists.
            if (::MileIsWindowsVersionAtLeast(10, 0, 14986))
            {
                return false;
            }

            return true;
        }());

        return CachedResult;
    }
}

EXTERN_C BOOL WINAPI MileIsWindowsVersionAtLeast(
    _In_ DWORD Major,
    _In_ DWORD Minor,
    _In_ DWORD Build)
{
    OSVERSIONINFOEXW OSVersionInfoEx = { 0 };
    OSVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    OSVersionInfoEx.dwMajorVersion = Major;
    OSVersionInfoEx.dwMinorVersion = Minor;
    OSVersionInfoEx.dwBuildNumber = Build;
    return ::VerifyVersionInfoW(
        &OSVersionInfoEx,
        VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER,
        ::VerSetConditionMask(
            ::VerSetConditionMask(
                ::VerSetConditionMask(
                    0,
                    VER_MAJORVERSION,
                    VER_GREATER_EQUAL),
                VER_MINORVERSION,
                VER_GREATER_EQUAL),
            VER_BUILDNUMBER,
            VER_GREATER_EQUAL));
}

EXTERN_C BOOL WINAPI MileEnablePerMonitorDialogScaling()
{
    if (!::IsPrivatePerMonitorSupportExtensionApplicable())
    {
        return FALSE;
    }

    HMODULE ModuleHandle = ::GetModuleHandleW(L"user32.dll");
    if (!ModuleHandle)
    {
        return FALSE;
    }

    // Reference: https://github.com/microsoft/terminal/blob
    //            /9b92986b49bed8cc41fde4d6ef080921c41e6d9e
    //            /src/interactivity/win32/windowdpiapi.cpp#L24
    typedef BOOL(WINAPI* ProcType)();

    ProcType ProcAddress = reinterpret_cast<ProcType>(
        ::GetProcAddress(ModuleHandle, reinterpret_cast<LPCSTR>(2577)));
    if (!ProcAddress)
    {
        return FALSE;
    }

    return ProcAddress();
}
