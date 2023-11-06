/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.Base.cpp
 * PURPOSE:   Implementation for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mile.Helpers.Base.h"

#include <assert.h>
#include <process.h>

EXTERN_C LPVOID WINAPI MileAllocateMemory(
    _In_ SIZE_T Size)
{
    return ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
}

EXTERN_C LPVOID WINAPI MileReallocateMemory(
    _In_ PVOID Block,
    _In_ SIZE_T Size)
{
    return ::HeapReAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, Block, Size);
}

EXTERN_C BOOL WINAPI MileFreeMemory(
    _In_ LPVOID Block)
{
    return ::HeapFree(::GetProcessHeap(), 0, Block);
}

namespace
{
    const NTSTATUS NtStatusSuccess = static_cast<NTSTATUS>(0x00000000L);
    const NTSTATUS NtStatusNotImplemented = static_cast<NTSTATUS>(0xC0000002L);

    static bool IsNtStatusSuccess(NTSTATUS Status)
    {
        return (Status >= 0);
    }

    typedef struct _NtUnicodeString
    {
        USHORT Length;
        USHORT MaximumLength;
        _Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;
    } NtUnicodeString, * NtUnicodeStringPointer;

    static HMODULE GetNtDllModuleHandle()
    {
        static HMODULE CachedResult = ::GetModuleHandleW(L"ntdll.dll");
        return CachedResult;
    }
}

namespace
{
    static FARPROC GetRtlGetVersionProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetNtDllModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "RtlGetVersion");
            }
            return nullptr;
        }());

        return CachedResult;
    }

    static NTSTATUS NTAPI RtlGetVersionWrapper(
        _Out_ PRTL_OSVERSIONINFOW VersionInformation)
    {
        // Reference: https://learn.microsoft.com/en-us/windows-hardware
        //            /drivers/ddi/wdm/nf-wdm-rtlgetversion
        using ProcType = decltype(::RtlGetVersionWrapper)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            ::GetRtlGetVersionProcAddress());

        if (ProcAddress)
        {
            return ProcAddress(VersionInformation);
        }

        return NtStatusNotImplemented;
    }
}

EXTERN_C BOOL WINAPI MileGetWindowsVersion(
    _Inout_ LPOSVERSIONINFOW VersionInformation)
{
    if (NtStatusSuccess == ::RtlGetVersionWrapper(VersionInformation))
    {
        return TRUE;
    }

    // No fallback solution due to RtlGetVersion has been introduced
    // since Windows 2000 and no interest to support earlier Windows
    // version than that.
    return FALSE;
}

namespace
{
     static LPOSVERSIONINFOW GetWindowsVersionInformation()
     {
        static LPOSVERSIONINFOW CachedResult = ([]() -> LPOSVERSIONINFOW
        {
            static OSVERSIONINFOW VersionInformation = { 0 };
            VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);

            if (::MileGetWindowsVersion(&VersionInformation))
            {
                return &VersionInformation;
            }

            return nullptr;
        }());

        return CachedResult;
    }
}

EXTERN_C BOOL WINAPI MileIsWindowsVersionAtLeast(
    _In_ DWORD Major,
    _In_ DWORD Minor,
    _In_ DWORD Build)
{
    LPOSVERSIONINFOW VersionInformation = ::GetWindowsVersionInformation();
    if (!VersionInformation)
    {
        return FALSE;
    }

    // Reference: https://github.com/dotnet/runtime/blob
    //            /db213657acc53fc48212867d5728e83d9e36a558
    //            /src/libraries/System.Private.CoreLib
    //            /src/System/OperatingSystem.cs#L308
    if (VersionInformation->dwMajorVersion != Major)
    {
        return (VersionInformation->dwMajorVersion > Major);
    }
    if (VersionInformation->dwMinorVersion != Minor)
    {
        return (VersionInformation->dwMinorVersion > Minor);
    }
    return (VersionInformation->dwBuildNumber >= Build);
}

EXTERN_C ULONGLONG WINAPI MileGetTickCount()
{
    LARGE_INTEGER Frequency;
    if (::QueryPerformanceFrequency(&Frequency))
    {
        LARGE_INTEGER PerformanceCount;
        if (::QueryPerformanceCounter(&PerformanceCount))
        {
            return (PerformanceCount.QuadPart * 1000 / Frequency.QuadPart);
        }
    }

    return ::GetTickCount64();
}

EXTERN_C HANDLE WINAPI MileCreateThread(
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ SIZE_T dwStackSize,
    _In_ LPTHREAD_START_ROUTINE lpStartAddress,
    _In_opt_ LPVOID lpParameter,
    _In_ DWORD dwCreationFlags,
    _Out_opt_ LPDWORD lpThreadId)
{
    // sanity check for lpThreadId
    assert(sizeof(DWORD) == sizeof(unsigned));

    typedef unsigned(__stdcall* routine_type)(void*);

    // _beginthreadex calls CreateThread which will set the last error
    // value before it returns.
    return reinterpret_cast<HANDLE>(::_beginthreadex(
        lpThreadAttributes,
        static_cast<unsigned>(dwStackSize),
        reinterpret_cast<routine_type>(lpStartAddress),
        lpParameter,
        dwCreationFlags,
        reinterpret_cast<unsigned*>(lpThreadId)));
}

EXTERN_C DWORD WINAPI MileGetNumberOfHardwareThreads()
{
    SYSTEM_INFO SystemInfo;
    ::GetNativeSystemInfo(&SystemInfo);
    return SystemInfo.dwNumberOfProcessors;
}

namespace
{
    static FARPROC GetLdrLoadDllProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetNtDllModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "LdrLoadDll");
            }
            return nullptr;
        }());

        return CachedResult;
    }

    static FARPROC GetRtlNtStatusToDosErrorProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetNtDllModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "RtlNtStatusToDosError");
            }
            return nullptr;
        }());

        return CachedResult;
    }

    static FARPROC GetRtlWow64EnableFsRedirectionExProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetNtDllModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "RtlWow64EnableFsRedirectionEx");
            }
            return nullptr;
        }());

        return CachedResult;
    }

    static FARPROC GetRtlInitUnicodeStringProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetNtDllModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "RtlInitUnicodeString");
            }
            return nullptr;
        }());

        return CachedResult;
    }

    static NTSTATUS NTAPI LdrLoadDllWrapper(
        _In_opt_ PWSTR DllPath,
        _In_opt_ PULONG DllCharacteristics,
        _In_ NtUnicodeStringPointer DllName,
        _Out_ PVOID* DllHandle)
    {
        using ProcType = decltype(::LdrLoadDllWrapper)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            ::GetLdrLoadDllProcAddress());

        if (ProcAddress)
        {
            return ProcAddress(
                DllPath,
                DllCharacteristics,
                DllName,
                DllHandle);
        }

        return NtStatusNotImplemented;
    }

    static ULONG NTAPI RtlNtStatusToDosErrorWrapper(
        _In_ NTSTATUS Status)
    {
        using ProcType = decltype(::RtlNtStatusToDosErrorWrapper)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            ::GetRtlNtStatusToDosErrorProcAddress());

        if (ProcAddress)
        {
            return ProcAddress(Status);
        }

        return ERROR_PROC_NOT_FOUND;
    }

    static NTSTATUS NTAPI RtlWow64EnableFsRedirectionExWrapper(
        _In_opt_ PVOID Wow64FsEnableRedirection,
        _Out_opt_ PVOID* OldFsRedirectionLevel)
    {
        using ProcType = decltype(::RtlWow64EnableFsRedirectionExWrapper)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            ::GetRtlWow64EnableFsRedirectionExProcAddress());

        if (ProcAddress)
        {
            return ProcAddress(
                Wow64FsEnableRedirection,
                OldFsRedirectionLevel);
        }

        return NtStatusNotImplemented;
    }

    static void NTAPI RtlInitUnicodeStringWrapper(
        _Out_ NtUnicodeStringPointer DestinationString,
        _In_opt_ PCWSTR SourceString)
    {
        if (!DestinationString)
        {
            return;
        }
        DestinationString->Length = 0;
        DestinationString->MaximumLength = 0;
        DestinationString->Buffer = nullptr;

        using ProcType = decltype(::RtlInitUnicodeStringWrapper)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            ::GetRtlInitUnicodeStringProcAddress());
        if (ProcAddress)
        {
            ProcAddress(DestinationString, SourceString);
        }
    }

    static bool IsSecureLibraryLoaderAvailable()
    {
        static bool CachedResult = ([]() -> bool
        {
            // We should check the secure library loader by get the address of
            // some APIs existed when the secure library loader is available.
            // Because some environment will return the ERROR_ACCESS_DENIED
            // instead of ERROR_INVALID_PARAMETER from GetLastError after
            // calling the LoadLibraryEx with using the unsupported flags.
            HMODULE ModuleHandle = ::GetModuleHandleW(L"kernel32.dll");
            if (ModuleHandle)
            {
                return ::GetProcAddress(ModuleHandle, "AddDllDirectory");
            }

            return false;
        }());

        return CachedResult;
    }
}

EXTERN_C HMODULE WINAPI MileLoadLibraryFromSystem32(
    _In_ LPCWSTR lpLibFileName)
{
    // The secure library loader is available when you using Windows 8 and
    // later, or you have installed the KB2533623 when you using Windows Vista
    // and 7.
    if (::IsSecureLibraryLoaderAvailable())
    {
        return ::LoadLibraryExW(
            lpLibFileName,
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    // We should re-enable the WoW64 redirection because Windows 7 RTM or
    // earlier won't re-enable the WoW64 redirection when loading the library.
    // It's vulnerable if someone put the malicious library under the native
    // system directory.
    PVOID OldRedirectionLevel = nullptr;
    NTSTATUS RedirectionStatus = ::RtlWow64EnableFsRedirectionExWrapper(
        nullptr,
        &OldRedirectionLevel);

    wchar_t System32Directory[MAX_PATH];
    UINT Length = ::GetSystemDirectoryW(System32Directory, MAX_PATH);
    if (Length == 0 || Length >= MAX_PATH)
    {
        // The length of the system directory path string (%windows%\system32)
        // should be shorter than the MAX_PATH constant.
        ::SetLastError(ERROR_FUNCTION_FAILED);
        return nullptr;
    }

    NtUnicodeString ModuleFileName;
    ::RtlInitUnicodeStringWrapper(&ModuleFileName, lpLibFileName);

    HMODULE ModuleHandle = nullptr;
    NTSTATUS Status = ::LdrLoadDllWrapper(
        System32Directory,
        nullptr,
        &ModuleFileName,
        reinterpret_cast<PVOID*>(&ModuleHandle));
    if (!IsNtStatusSuccess(Status))
    {
        ::SetLastError(::RtlNtStatusToDosErrorWrapper(Status));
    }

    // Restore the old status of the WoW64 redirection.
    if (IsNtStatusSuccess(RedirectionStatus))
    {
        ::RtlWow64EnableFsRedirectionExWrapper(
            OldRedirectionLevel,
            &OldRedirectionLevel);
    }

    return ModuleHandle;
}

EXTERN_C BOOL WINAPI MileIsCurrentProcessElevated()
{
    bool Result = false;

    HANDLE CurrentProcessAccessToken = nullptr;
    if (::OpenProcessToken(
        ::GetCurrentProcess(),
        TOKEN_ALL_ACCESS,
        &CurrentProcessAccessToken))
    {
        TOKEN_ELEVATION Information = { 0 };
        DWORD Length = sizeof(Information);
        if (::GetTokenInformation(
            CurrentProcessAccessToken,
            TOKEN_INFORMATION_CLASS::TokenElevation,
            &Information,
            Length,
            &Length))
        {
            Result = Information.TokenIsElevated;
        }

        ::CloseHandle(CurrentProcessAccessToken);
    }

    return Result ? TRUE : FALSE;
}
