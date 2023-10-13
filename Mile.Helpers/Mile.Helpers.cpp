/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.cpp
 * PURPOSE:   Implementation for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#include "Mile.Helpers.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <Uxtheme.h>
#pragma comment(lib, "Uxtheme.lib")

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

    static bool IsWindows10Version1809OrLater()
    {
        static bool CachedResult = ::MileIsWindowsVersionAtLeast(10, 0, 17763);
        return CachedResult;
    }

    static bool IsWindows10Version1903OrLater()
    {
        static bool CachedResult = ::MileIsWindowsVersionAtLeast(10, 0, 18362);
        return CachedResult;
    }

    static bool IsWindows10Version20H1OrLater()
    {
        static bool CachedResult = ::MileIsWindowsVersionAtLeast(10, 0, 19041);
        return CachedResult;
    }

    static bool IsWindows11Version21H2OrLater()
    {
        static bool CachedResult = ::MileIsWindowsVersionAtLeast(10, 0, 22000);
        return CachedResult;
    }

    static HMODULE GetUxThemeModuleHandle()
    {
        static HMODULE CachedResult = ::LoadLibraryExW(
            L"uxtheme.dll",
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
        return CachedResult;
    }

    static FARPROC GetUxThemeOrdinal135ProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetUxThemeModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    reinterpret_cast<LPCSTR>(135));
            }
            return nullptr;
        }());

        return CachedResult;
    }
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

EXTERN_C HRESULT WINAPI MileSetWindowCaptionColorAttribute(
    _In_ HWND WindowHandle,
    _In_ COLORREF Value)
{
    return ::DwmSetWindowAttribute(
        WindowHandle,
        DWMWA_CAPTION_COLOR,
        &Value,
        sizeof(COLORREF));
}

EXTERN_C HRESULT WINAPI MileSetWindowUseImmersiveDarkModeAttribute(
    _In_ HWND WindowHandle,
    _In_ BOOL Value)
{
    const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;
    return ::DwmSetWindowAttribute(
        WindowHandle,
        (::IsWindows10Version20H1OrLater()
            ? DWMWA_USE_IMMERSIVE_DARK_MODE
            : DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1),
        &Value,
        sizeof(BOOL));
}

EXTERN_C HRESULT WINAPI MileGetWindowSystemBackdropTypeAttribute(
    _In_ HWND WindowHandle,
    _Out_ PMILE_WINDOW_SYSTEM_BACKDROP_TYPE Value)
{
    if (!Value)
    {
        return E_INVALIDARG;
    }

    return ::DwmGetWindowAttribute(
        WindowHandle,
        DWMWA_SYSTEMBACKDROP_TYPE,
        Value,
        sizeof(MILE_WINDOW_SYSTEM_BACKDROP_TYPE));
}

EXTERN_C HRESULT WINAPI MileSetWindowSystemBackdropTypeAttribute(
    _In_ HWND WindowHandle,
    _In_ MILE_WINDOW_SYSTEM_BACKDROP_TYPE Value)
{
    HRESULT hr = S_OK;

    if (MILE_WINDOW_SYSTEM_BACKDROP_TYPE_AUTO != Value &&
        MILE_WINDOW_SYSTEM_BACKDROP_TYPE_NONE != Value)
    {
        // Reset the caption color of a window before apply the backdrop.
        hr = ::MileSetWindowCaptionColorAttribute(
            WindowHandle,
            DWMWA_COLOR_DEFAULT);
    }

    if (S_OK == hr)
    {
        hr = ::DwmSetWindowAttribute(
            WindowHandle,
            DWMWA_SYSTEMBACKDROP_TYPE,
            &Value,
            sizeof(MILE_WINDOW_SYSTEM_BACKDROP_TYPE));
    }

    return hr;
}

EXTERN_C MILE_PREFERRED_APP_MODE WINAPI MileSetPreferredAppMode(
    _In_ MILE_PREFERRED_APP_MODE NewMode)
{
    if (!::IsWindows10Version1903OrLater())
    {
        return MILE_PREFERRED_APP_MODE_DEFAULT;
    }

    typedef MILE_PREFERRED_APP_MODE(WINAPI* ProcType)(MILE_PREFERRED_APP_MODE);
    ProcType ProcAddress = reinterpret_cast<ProcType>(
        ::GetUxThemeOrdinal135ProcAddress());
    if (!ProcAddress)
    {
        return MILE_PREFERRED_APP_MODE_DEFAULT;
    }

    return ProcAddress(NewMode);
}

EXTERN_C BOOL WINAPI MileAllowDarkModeForApp(
    _In_ BOOL NewPolicy)
{
    if (::IsWindows10Version1809OrLater())
    {
        if (::IsWindows10Version1903OrLater())
        {
            return (MILE_PREFERRED_APP_MODE_AUTO == ::MileSetPreferredAppMode(
                NewPolicy
                ? MILE_PREFERRED_APP_MODE_AUTO
                : MILE_PREFERRED_APP_MODE_DEFAULT));
        }
        else
        {
            // Use original AllowDarkModeForApp
            typedef BOOL(WINAPI* ProcType)(BOOL);
            ProcType ProcAddress = reinterpret_cast<ProcType>(
                ::GetUxThemeOrdinal135ProcAddress());
            if (ProcAddress)
            {
                return ProcAddress(NewPolicy);
            }
        }
    }

    return false;
}

EXTERN_C COLORREF WINAPI MileGetDefaultBackgroundColorValue(
    _In_ BOOL UseImmersiveDarkMode)
{
    return ::IsWindows11Version21H2OrLater()
        ? (UseImmersiveDarkMode ? RGB(32, 32, 32) : RGB(243, 243, 243))
        : (UseImmersiveDarkMode ? RGB(0, 0, 0) : RGB(255, 255, 255));
}

EXTERN_C HRESULT WINAPI MileEnableImmersiveDarkModeForWindow(
    _In_ HWND WindowHandle,
    _In_ BOOL UseImmersiveDarkMode)
{
    HRESULT hr = ::MileSetWindowUseImmersiveDarkModeAttribute(
        WindowHandle,
        UseImmersiveDarkMode);
    if (S_OK == hr)
    {
        bool NeedFallback = true;

        MILE_WINDOW_SYSTEM_BACKDROP_TYPE Type =
            MILE_WINDOW_SYSTEM_BACKDROP_TYPE_AUTO;
        hr = ::MileGetWindowSystemBackdropTypeAttribute(
            WindowHandle,
            &Type);
        if (S_OK == hr)
        {
            if (MILE_WINDOW_SYSTEM_BACKDROP_TYPE_AUTO != Type &&
                MILE_WINDOW_SYSTEM_BACKDROP_TYPE_NONE != Type)
            {
                NeedFallback = false;
            }
        }

        if (NeedFallback)
        {
            hr = ::MileSetWindowCaptionColorAttribute(
                WindowHandle,
                ::MileGetDefaultBackgroundColorValue(UseImmersiveDarkMode));
        }
    }

    return hr;
}

EXTERN_C HRESULT WINAPI MileAllowNonClientDefaultDrawingForWindow(
    _In_ HWND WindowHandle,
    _In_ BOOL NewPolicy)
{
    WTA_OPTIONS Attribute = { 0 };
    Attribute.dwMask = WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON;
    Attribute.dwFlags = NewPolicy ? 0 : Attribute.dwMask;
    return ::SetWindowThemeAttribute(
        WindowHandle,
        WTA_NONCLIENT,
        &Attribute,
        sizeof(WTA_OPTIONS));
}
