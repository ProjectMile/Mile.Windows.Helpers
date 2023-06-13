﻿/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.h
 * PURPOSE:   Definition for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#pragma once

#ifndef MILE_WINDOWS_HELPERS
#define MILE_WINDOWS_HELPERS

#include "Mile.Helpers.Base.h"

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
 * @brief Flags for specifying the preferred app color mode.
*/
typedef enum MILE_PREFERRED_APP_MODE
{
    /**
     * @brief Use the default behavior.
    */
    MILE_PREFERRED_APP_MODE_DEFAULT = 0,

    /**
     * @brief Let the OS decide to use light mode or dark mode.
    */
    MILE_PREFERRED_APP_MODE_AUTO = 1,

    /**
     * @brief Force dark mode.
    */
    MILE_PREFERRED_APP_MODE_DARK = 2,

    /**
     * @brief Force light mode.
    */
    MILE_PREFERRED_APP_MODE_LIGHT = 3

} MILE_PREFERRED_APP_MODE;

/**
 * @brief Values that specify the type of privileges being assigned to or
 *        retrieved from an access token.
 * @remark Definitions are synchronized from in wdm.h of Windows Driver Kit.
 *         These must be converted to LUIDs before use.
*/
typedef enum MILE_TOKEN_PRIVILEGE_LUID_CLASS
{
    /**
     * @brief The minimal value for this enumeration.
    */
    MILE_TOKEN_MIN_WELL_KNOWN_PRIVILEGE = 2,

    /**
     * @brief Create a token object.
    */
    MILE_TOKEN_CREATE_TOKEN_PRIVILEGE = 2,

    /**
     * @brief Replace a process-level token.
    */
    MILE_TOKEN_ASSIGNPRIMARYTOKEN_PRIVILEGE = 3,

    /**
     * @brief Lock pages in memory.
    */
    MILE_TOKEN_LOCK_MEMORY_PRIVILEGE = 4,

    /**
     * @brief Adjust memory quotas for a process.
    */
    MILE_TOKEN_INCREASE_QUOTA_PRIVILEGE = 5,

    /**
     * @brief Add workstations to domain.
    */
    MILE_TOKEN_MACHINE_ACCOUNT_PRIVILEGE = 6,

    /**
     * @brief Act as part of the operating system.
    */
    MILE_TOKEN_TCB_PRIVILEGE = 7,

    /**
     * @brief Manage auditing and security log.
    */
    MILE_TOKEN_SECURITY_PRIVILEGE = 8,

    /**
     * @brief Take ownership of files or other objects.
    */
    MILE_TOKEN_TAKE_OWNERSHIP_PRIVILEGE = 9,

    /**
     * @brief Load and unload device drivers.
    */
    MILE_TOKEN_LOAD_DRIVER_PRIVILEGE = 10,

    /**
     * @brief Profile system performance.
    */
    MILE_TOKEN_SYSTEM_PROFILE_PRIVILEGE = 11,

    /**
     * @brief Change the system time.
    */
    MILE_TOKEN_SYSTEMTIME_PRIVILEGE = 12,

    /**
     * @brief Profile single process.
    */
    MILE_TOKEN_PROF_SINGLE_PROCESS_PRIVILEGE = 13,

    /**
     * @brief Increase scheduling priority.
    */
    MILE_TOKEN_INC_BASE_PRIORITY_PRIVILEGE = 14,

    /**
     * @brief Create a pagefile.
    */
    MILE_TOKEN_CREATE_PAGEFILE_PRIVILEGE = 15,

    /**
     * @brief Create permanent shared objects.
    */
    MILE_TOKEN_CREATE_PERMANENT_PRIVILEGE = 16,

    /**
     * @brief Back up files and directories.
    */
    MILE_TOKEN_BACKUP_PRIVILEGE = 17,

    /**
     * @brief Restore files and directories.
    */
    MILE_TOKEN_RESTORE_PRIVILEGE = 18,

    /**
     * @brief Shut down the system.
    */
    MILE_TOKEN_SHUTDOWN_PRIVILEGE = 19,

    /**
     * @brief Debug programs.
    */
    MILE_TOKEN_DEBUG_PRIVILEGE = 20,

    /**
     * @brief Generate security audits.
    */
    MILE_TOKEN_AUDIT_PRIVILEGE = 21,

    /**
     * @brief Modify firmware environment values.
    */
    MILE_TOKEN_SYSTEM_ENVIRONMENT_PRIVILEGE = 22,

    /**
     * @brief Bypass traverse checking.
    */
    MILE_TOKEN_CHANGE_NOTIFY_PRIVILEGE = 23,

    /**
     * @brief Force shutdown from a remote system.
    */
    MILE_TOKEN_REMOTE_SHUTDOWN_PRIVILEGE = 24,

    /**
     * @brief Remove computer from docking station.
    */
    MILE_TOKEN_UNDOCK_PRIVILEGE = 25,

    /**
     * @brief Synchronize directory service data.
    */
    MILE_TOKEN_SYNC_AGENT_PRIVILEGE = 26,

    /**
     * @brief Enable computer and user accounts to be trusted for delegation.
    */
    MILE_TOKEN_ENABLE_DELEGATION_PRIVILEGE = 27,

    /**
     * @brief Manage the files on a volume.
    */
    MILE_TOKEN_MANAGE_VOLUME_PRIVILEGE = 28,

    /**
     * @brief Impersonate a client after authentication.
    */
    MILE_TOKEN_IMPERSONATE_PRIVILEGE = 29,

    /**
     * @brief Create global objects.
    */
    MILE_TOKEN_CREATE_GLOBAL_PRIVILEGE = 30,

    /**
     * @brief Access Credential Manager as a trusted caller.
    */
    MILE_TOKEN_TRUSTED_CREDMAN_ACCESS_PRIVILEGE = 31,

    /**
     * @brief Modify an object label.
    */
    MILE_TOKEN_RELABEL_PRIVILEGE = 32,

    /**
     * @brief Increase a process working set.
    */
    MILE_TOKEN_INC_WORKING_SET_PRIVILEGE = 33,

    /**
     * @brief Change the time zone.
    */
    MILE_TOKEN_TIME_ZONE_PRIVILEGE = 34,

    /**
     * @brief Create symbolic links.
    */
    MILE_TOKEN_CREATE_SYMBOLIC_LINK_PRIVILEGE = 35,

    /**
     * @brief Impersonate other users.
    */
    MILE_TOKEN_DELEGATE_SESSION_USER_IMPERSONATE_PRIVILEGE = 36,

    /**
     * @brief The maximum value for this enumeration.
    */
    MILE_TOKEN_MAX_WELL_KNOWN_PRIVILEGE = 36

} MILE_TOKEN_PRIVILEGE_LUID_CLASS;

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

/**
 * @brief Retrieves the system-drawn backdrop material of a window, including
 *        behind the non-client area.
 * @param WindowHandle The handle to the window for which the attribute value
 *                     is to be set.
 * @param Value Flags for specifying the system-drawn backdrop material of a
 *              window, including behind the non-client area.
 * @return If the function succeeds, it returns S_OK. Otherwise, it returns an
 *         HRESULT error code.
*/
EXTERN_C HRESULT WINAPI MileGetWindowSystemBackdropTypeAttribute(
    _In_ HWND WindowHandle,
    _Out_ PMILE_WINDOW_SYSTEM_BACKDROP_TYPE Value);

/**
 * @brief Specifies the system-drawn backdrop material of a window, including
 *        behind the non-client area.
 * @param WindowHandle The handle to the window for which the attribute value
 *                     is to be set.
 * @param Value Flags for specifying the system-drawn backdrop material of a
 *              window, including behind the non-client area.
 * @return If the function succeeds, it returns S_OK. Otherwise, it returns an
 *         HRESULT error code.
*/
EXTERN_C HRESULT WINAPI MileSetWindowSystemBackdropTypeAttribute(
    _In_ HWND WindowHandle,
    _In_ MILE_WINDOW_SYSTEM_BACKDROP_TYPE Value);

/**
 * @brief Specifies the preferred app color mode.
 * @param NewMode The new preferred app color mode.
 * @return The previous preferred app color mode.
*/
EXTERN_C MILE_PREFERRED_APP_MODE WINAPI MileSetPreferredAppMode(
    _In_ MILE_PREFERRED_APP_MODE NewMode);

/**
 * @brief Specifies the policy for enabling or disabling dark mode for app.
 * @param NewPolicy The policy for enabling or disabling dark mode for app.
 * @return The previous policy for enabling or disabling dark mode for app.
*/
EXTERN_C BOOL WINAPI MileAllowDarkModeForApp(
    _In_ BOOL NewPolicy);

/**
 * @brief Gets the default background color value.
 * @param UseImmersiveDarkMode TRUE to use dark mode, FALSE to use light mode.
 * @return The default background color value.
*/
EXTERN_C COLORREF WINAPI MileGetDefaultBackgroundColorValue(
    _In_ BOOL UseImmersiveDarkMode);

/**
 * @brief Enable the dark mode or light mode policy for the window.
 * @param WindowHandle The handle to the window for which the attribute value
 *                     is to be set.
 * @param UseImmersiveDarkMode TRUE to use dark mode, FALSE to use light mode.
 * @return If the function succeeds, it returns S_OK. Otherwise, it returns an
 *         HRESULT error code.
*/
EXTERN_C HRESULT WINAPI MileEnableImmersiveDarkModeForWindow(
    _In_ HWND WindowHandle,
    _In_ BOOL UseImmersiveDarkMode);

#endif // !MILE_WINDOWS_HELPERS
