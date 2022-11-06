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
