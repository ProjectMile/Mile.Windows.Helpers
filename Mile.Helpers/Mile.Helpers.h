/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.h
 * PURPOSE:   Definition for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#ifndef MILE_WINDOWS_HELPERS
#define MILE_WINDOWS_HELPERS

#include <Windows.h>

/**
 * @brief Indicates if the current OS version matches, or is greater than, the
 *        provided version information.
 * @param Major The major version number of the operating system.
 * @param Minor The minor version number of the operating system.
 * @param Build The build number of the operating system.
 * @return TRUE if the specified version matches, or is greater than, the
 *         version of the current Windows operating system; otherwise, FALSE.
*/
EXTERN_C BOOL WINAPI MileIsWindowsVersionAtLeast(
    _In_ DWORD Major,
    _In_ DWORD Minor,
    _In_ DWORD Build);

#endif // !MILE_WINDOWS_HELPERS
