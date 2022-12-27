/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.Base.cpp
 * PURPOSE:   Implementation for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
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
     static LPOSVERSIONINFOW GetWindowsVersionInformation()
     {
        static LPOSVERSIONINFOW CachedResult = ([]() -> LPOSVERSIONINFOW
        {
            static OSVERSIONINFOW VersionInformation = { 0 };
            VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);

            HMODULE ModuleHandle = ::GetModuleHandleW(L"ntdll.dll");
            if (ModuleHandle)
            {
                // Reference: https://learn.microsoft.com/en-us/windows-hardware
                //            /drivers/ddi/wdm/nf-wdm-rtlgetversion
                typedef NTSTATUS(NTAPI* ProcType)(PRTL_OSVERSIONINFOW);

                ProcType ProcAddress = reinterpret_cast<ProcType>(
                    ::GetProcAddress(ModuleHandle, "RtlGetVersion"));
                if (ProcAddress)
                {
                    // Use constant 0 due to STATUS_SUCCESS not defined in the
                    // standard Windows SDK.
                    if (0 == ProcAddress(&VersionInformation))
                    {
                        return &VersionInformation;
                    }
                }
            }

            // No fallback solution due to RtlGetVersion has been introduced
            // since Windows 2000 and no interest to support earlier Windows
            // version than that.
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
