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
 * @brief Flags for specifying the system-drawn backdrop material of a window,
 *        including behind the non-client area.
*/
typedef enum MILE_WINDOW_SYSTEM_BACKDROP_TYPE
{
    /**
     * @brief The default. Let the Desktop Window Manager (DWM) automatically
              decide the system-drawn backdrop material for this window.
    */
    MILE_WINDOW_SYSTEM_BACKDROP_TYPE_AUTO = 0,

    /**
     * @brief Don't draw any system backdrop.
    */
    MILE_WINDOW_SYSTEM_BACKDROP_TYPE_NONE = 1,

    /**
     * @brief Draw the backdrop material effect corresponding to a long-lived
     *        window.
    */
    MILE_WINDOW_SYSTEM_BACKDROP_TYPE_MICA = 2,

    /**
     * @brief Draw the backdrop material effect corresponding to a transient
     *        window.
    */
    MILE_WINDOW_SYSTEM_BACKDROP_TYPE_ACRYLIC = 3,

    /**
     * @brief Draw the backdrop material effect corresponding to a window with
     *        a tabbed title bar.
    */
    MILE_WINDOW_SYSTEM_BACKDROP_TYPE_MICA_ALT = 4

} MILE_WINDOW_SYSTEM_BACKDROP_TYPE, *PMILE_WINDOW_SYSTEM_BACKDROP_TYPE;

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

/**
 * @brief Enables automatic display scaling of the dialogs in high-DPI
 *        displays. Must be called before the creation of dialogs.
 * @return If the function succeeds, the return value is nonzero. If the
 *         function fails, the return value is zero. To get extended error
 *         information, call GetLastError.
 * @remarks Applications running at a DPI_AWARENESS_CONTEXT of
 *          DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 automatically scale
 *          their dialogs by default. They do not need to call this function.
 *          Starting from Windows 10 Build 14986, Microsoft introduces the
 *          Per-Monitor (V2) DPI Awareness.
 *          Reference: https://blogs.windows.com/windows-insider/2016/12/07/
 *                     announcing-windows-10-insider-preview-build-14986-pc
 *          The Windows Notepad in Windows 10 Build 14986 started to use
 *          Per-Monitor (V2) DPI Awareness.
*/
EXTERN_C BOOL WINAPI MileEnablePerMonitorDialogScaling();

/**
 * @brief Specifies the color of the caption for the window.
 * @param WindowHandle The handle to the window for which the attribute value
 *                     is to be set.
 * @param Value The color of the caption for the window.
 * @return If the function succeeds, it returns S_OK. Otherwise, it returns an
 *         HRESULT error code.
*/
EXTERN_C HRESULT WINAPI MileSetWindowCaptionColorAttribute(
    _In_ HWND WindowHandle,
    _In_ COLORREF Value);

/**
 * @brief Allows the window frame for this window to be drawn in dark mode
 *        colors when the dark mode system setting is enabled.
 * @param WindowHandle The handle to the window for which the attribute value
 *                     is to be set.
 * @param Value TRUE to honor dark mode for the window, FALSE to always use
 *              light mode.
 * @return If the function succeeds, it returns S_OK. Otherwise, it returns an
 *         HRESULT error code.
*/
EXTERN_C HRESULT WINAPI MileSetWindowUseImmersiveDarkModeAttribute(
    _In_ HWND WindowHandle,
    _In_ BOOL Value);

#endif // !MILE_WINDOWS_HELPERS
