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

#include "Mile.Helpers.CppBase.h"

#include <strsafe.h>

#include <cassert>
#include <cstring>

#include <process.h>

#include <string>

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

EXTERN_C BOOL WINAPI MileStartService(
    _In_ LPCWSTR ServiceName,
    _Out_ LPSERVICE_STATUS_PROCESS ServiceStatus)
{
    if (!ServiceName || !ServiceStatus)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    BOOL Result = FALSE;

    std::memset(ServiceStatus, 0, sizeof(LPSERVICE_STATUS_PROCESS));

    SC_HANDLE ServiceControlManagerHandle = ::OpenSCManagerW(
        nullptr,
        nullptr,
        SC_MANAGER_CONNECT);
    if (ServiceControlManagerHandle)
    {
        SC_HANDLE ServiceHandle = ::OpenServiceW(
            ServiceControlManagerHandle,
            ServiceName,
            SERVICE_QUERY_STATUS | SERVICE_START);
        if (ServiceHandle)
        {
            DWORD nBytesNeeded = 0;
            DWORD nOldCheckPoint = 0;
            ULONGLONG nLastTick = 0;
            bool bStartServiceWCalled = false;

            while (::QueryServiceStatusEx(
                ServiceHandle,
                SC_STATUS_PROCESS_INFO,
                reinterpret_cast<LPBYTE>(ServiceStatus),
                sizeof(SERVICE_STATUS_PROCESS),
                &nBytesNeeded))
            {
                if (SERVICE_RUNNING == ServiceStatus->dwCurrentState)
                {
                    Result = TRUE;
                    break;
                }
                else if (SERVICE_STOPPED == ServiceStatus->dwCurrentState)
                {
                    // Failed if the service had stopped again.
                    if (bStartServiceWCalled)
                    {
                        Result = FALSE;
                        ::SetLastError(ERROR_FUNCTION_FAILED);
                        break;
                    }

                    Result = ::StartServiceW(
                        ServiceHandle,
                        0,
                        nullptr);
                    if (!Result)
                    {
                        break;
                    }

                    bStartServiceWCalled = true;
                }
                else if (
                    SERVICE_STOP_PENDING == ServiceStatus->dwCurrentState ||
                    SERVICE_START_PENDING == ServiceStatus->dwCurrentState)
                {
                    ULONGLONG nCurrentTick = ::GetTickCount64();

                    if (!nLastTick)
                    {
                        nLastTick = nCurrentTick;
                        nOldCheckPoint = ServiceStatus->dwCheckPoint;

                        // Same as the .Net System.ServiceProcess, wait
                        // 250ms.
                        ::SleepEx(250, FALSE);
                    }
                    else
                    {
                        // Check the timeout if the checkpoint is not
                        // increased.
                        if (ServiceStatus->dwCheckPoint
                            <= nOldCheckPoint)
                        {
                            ULONGLONG nDiff = nCurrentTick - nLastTick;
                            if (nDiff > ServiceStatus->dwWaitHint)
                            {
                                Result = FALSE;
                                ::SetLastError(ERROR_TIMEOUT);
                                break;
                            }
                        }

                        // Continue looping.
                        nLastTick = 0;
                    }
                }
                else
                {
                    break;
                }
            }

            ::CloseServiceHandle(ServiceHandle);
        }

        ::CloseServiceHandle(ServiceControlManagerHandle);
    }

    return Result;
}

namespace
{
    static void FillFileEnumerateInformation(
        _In_ PFILE_ID_BOTH_DIR_INFO OriginalInformation,
        _Out_ PMILE_FILE_ENUMERATE_INFORMATION ConvertedInformation)
    {
        ConvertedInformation->CreationTime.dwLowDateTime =
            OriginalInformation->CreationTime.LowPart;
        ConvertedInformation->CreationTime.dwHighDateTime =
            OriginalInformation->CreationTime.HighPart;

        ConvertedInformation->LastAccessTime.dwLowDateTime =
            OriginalInformation->LastAccessTime.LowPart;
        ConvertedInformation->LastAccessTime.dwHighDateTime =
            OriginalInformation->LastAccessTime.HighPart;

        ConvertedInformation->LastWriteTime.dwLowDateTime =
            OriginalInformation->LastWriteTime.LowPart;
        ConvertedInformation->LastWriteTime.dwHighDateTime =
            OriginalInformation->LastWriteTime.HighPart;

        ConvertedInformation->ChangeTime.dwLowDateTime =
            OriginalInformation->ChangeTime.LowPart;
        ConvertedInformation->ChangeTime.dwHighDateTime =
            OriginalInformation->ChangeTime.HighPart;

        ConvertedInformation->FileSize =
            OriginalInformation->EndOfFile.QuadPart;

        ConvertedInformation->AllocationSize =
            OriginalInformation->AllocationSize.QuadPart;

        ConvertedInformation->FileAttributes =
            OriginalInformation->FileAttributes;

        ConvertedInformation->EaSize =
            OriginalInformation->EaSize;

        ConvertedInformation->FileId =
            OriginalInformation->FileId;

        ::StringCbCopyNW(
            ConvertedInformation->ShortName,
            sizeof(ConvertedInformation->ShortName),
            OriginalInformation->ShortName,
            OriginalInformation->ShortNameLength);

        ::StringCbCopyNW(
            ConvertedInformation->FileName,
            sizeof(ConvertedInformation->FileName),
            OriginalInformation->FileName,
            OriginalInformation->FileNameLength);
    }
}

EXTERN_C BOOL WINAPI MileEnumerateFileByHandle(
    _In_ HANDLE FileHandle,
    _In_ MILE_ENUMERATE_FILE_CALLBACK_TYPE Callback,
    _In_opt_ LPVOID Context)
{
    if (!FileHandle || FileHandle == INVALID_HANDLE_VALUE || !Callback)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    const SIZE_T BufferSize = 32768;
    PBYTE Buffer = nullptr;
    PFILE_ID_BOTH_DIR_INFO OriginalInformation = nullptr;
    MILE_FILE_ENUMERATE_INFORMATION ConvertedInformation = { 0 };

    auto ExitHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (Buffer)
        {
            ::MileFreeMemory(Buffer);
        }
    });

    Buffer = reinterpret_cast<PBYTE>(::MileAllocateMemory(BufferSize));
    if (!Buffer)
    {
        ::SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
    }

    OriginalInformation = reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(Buffer);

    if (::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileIdBothDirectoryRestartInfo,
        OriginalInformation,
        BufferSize))
    {
        for (;;)
        {
            ::FillFileEnumerateInformation(
                OriginalInformation,
                &ConvertedInformation);

            if (!Callback(&ConvertedInformation, Context))
            {
                return TRUE;
            }

            if (!OriginalInformation->NextEntryOffset)
            {
                OriginalInformation =
                    reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(Buffer);
                break;
            }

            OriginalInformation = reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(
                reinterpret_cast<ULONG_PTR>(OriginalInformation)
                + OriginalInformation->NextEntryOffset);
        }

        while (::GetFileInformationByHandleEx(
            FileHandle,
            FILE_INFO_BY_HANDLE_CLASS::FileIdBothDirectoryInfo,
            OriginalInformation,
            BufferSize))
        {
            for (;;)
            {
                ::FillFileEnumerateInformation(
                    OriginalInformation,
                    &ConvertedInformation);

                if (!Callback(&ConvertedInformation, Context))
                {
                    return TRUE;
                }

                if (!OriginalInformation->NextEntryOffset)
                {
                    OriginalInformation =
                        reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(Buffer);
                    break;
                }

                OriginalInformation = reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(
                    reinterpret_cast<ULONG_PTR>(OriginalInformation)
                    + OriginalInformation->NextEntryOffset);
            }
        }
    }

    return (ERROR_NO_MORE_FILES == ::GetLastError());
}

EXTERN_C BOOL WINAPI MileDeviceIoControl(
    _In_ HANDLE DeviceHandle,
    _In_ DWORD IoControlCode,
    _In_opt_ LPVOID InputBuffer,
    _In_ DWORD InputBufferSize,
    _Out_opt_ LPVOID OutputBuffer,
    _In_ DWORD OutputBufferSize,
    _Out_opt_ LPDWORD BytesReturned)
{
    BOOL Result = FALSE;
    DWORD NumberOfBytesTransferred = 0;
    OVERLAPPED Overlapped = { 0 };
    Overlapped.hEvent = ::CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr);
    if (Overlapped.hEvent)
    {
        Result = ::DeviceIoControl(
            DeviceHandle,
            IoControlCode,
            InputBuffer,
            InputBufferSize,
            OutputBuffer,
            OutputBufferSize,
            &NumberOfBytesTransferred,
            &Overlapped);
        if (!Result)
        {
            if (ERROR_IO_PENDING == ::GetLastError())
            {
                Result = ::GetOverlappedResult(
                    DeviceHandle,
                    &Overlapped,
                    &NumberOfBytesTransferred,
                    TRUE);
            }
        }

        ::CloseHandle(Overlapped.hEvent);
    }
    else
    {
        ::SetLastError(ERROR_NO_SYSTEM_RESOURCES);
    }

    if (BytesReturned)
    {
        *BytesReturned = NumberOfBytesTransferred;
    }

    return Result;
}

EXTERN_C BOOL WINAPI MileGetFileAttributesByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PDWORD FileAttributes)
{
    FILE_BASIC_INFO BasicInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo,
        &BasicInfo,
        sizeof(FILE_BASIC_INFO));

    *FileAttributes = Result
        ? BasicInfo.FileAttributes
        : INVALID_FILE_ATTRIBUTES;

    return Result;
}

EXTERN_C BOOL WINAPI MileSetFileAttributesByHandle(
    _In_ HANDLE FileHandle,
    _In_ DWORD FileAttributes)
{
    FILE_BASIC_INFO BasicInfo = { 0 };
    BasicInfo.FileAttributes =
        FileAttributes & (
            FILE_SHARE_READ |
            FILE_SHARE_WRITE |
            FILE_SHARE_DELETE |
            FILE_ATTRIBUTE_ARCHIVE |
            FILE_ATTRIBUTE_TEMPORARY |
            FILE_ATTRIBUTE_OFFLINE |
            FILE_ATTRIBUTE_NOT_CONTENT_INDEXED |
            FILE_ATTRIBUTE_NO_SCRUB_DATA) |
        FILE_ATTRIBUTE_NORMAL;

    return ::SetFileInformationByHandle(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo,
        &BasicInfo,
        sizeof(FILE_BASIC_INFO));
}

EXTERN_C BOOL WINAPI MileGetFileHardlinkCountByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PDWORD HardlinkCount)
{
    FILE_STANDARD_INFO StandardInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
        &StandardInfo,
        sizeof(FILE_STANDARD_INFO));

    *HardlinkCount = Result
        ? StandardInfo.NumberOfLinks
        : static_cast<DWORD>(-1);

    return Result;
}

EXTERN_C BOOL WINAPI MileDeleteFileByHandle(
    _In_ HANDLE FileHandle)
{
    FILE_DISPOSITION_INFO DispostionInfo;
    DispostionInfo.DeleteFile = TRUE;
    if (::SetFileInformationByHandle(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileDispositionInfo,
        &DispostionInfo,
        sizeof(FILE_DISPOSITION_INFO)))
    {
        return TRUE;
    }

    FILE_DISPOSITION_INFO_EX DispostionInfoEx;
    DispostionInfoEx.Flags = FILE_DISPOSITION_FLAG_DELETE;
    return ::SetFileInformationByHandle(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileDispositionInfoEx,
        &DispostionInfoEx,
        sizeof(FILE_DISPOSITION_INFO_EX));
}

EXTERN_C BOOL WINAPI MileDeleteFileIgnoreReadonlyAttributeByHandle(
    _In_ HANDLE FileHandle)
{
    BOOL Result = FALSE;

    DWORD OldAttribute = 0;
    // Save old attributes.
    if (::MileGetFileAttributesByHandle(FileHandle, &OldAttribute))
    {
        // Get hardlink count.
        DWORD HardlinkCount = 0;
        if (::MileGetFileHardlinkCountByHandle(FileHandle, &HardlinkCount))
        {
            // Remove readonly attribute.
            if (::MileSetFileAttributesByHandle(
                FileHandle,
                OldAttribute & (-1 ^ FILE_ATTRIBUTE_READONLY)))
            {
                // Delete the file.
                Result = ::MileDeleteFileByHandle(FileHandle);
                if (!Result || HardlinkCount > 1)
                {
                    // Restore attributes if failed or had another hard links.
                    ::MileSetFileAttributesByHandle(FileHandle, OldAttribute);
                }
            }
        }
    }

    return Result;
}

EXTERN_C BOOL WINAPI MileGetFileSizeByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PULONGLONG FileSize)
{
    FILE_STANDARD_INFO StandardInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
        &StandardInfo,
        sizeof(FILE_STANDARD_INFO));

    *FileSize = Result
        ? static_cast<ULONGLONG>(StandardInfo.EndOfFile.QuadPart)
        : 0;

    return Result;
}

EXTERN_C BOOL WINAPI MileGetFileAllocationSizeByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PULONGLONG AllocationSize)
{
    FILE_STANDARD_INFO StandardInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
        &StandardInfo,
        sizeof(FILE_STANDARD_INFO));

    *AllocationSize = Result
        ? static_cast<ULONGLONG>(StandardInfo.AllocationSize.QuadPart)
        : 0;

    return Result;
}

EXTERN_C BOOL WINAPI MileGetCompressedFileSizeByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PULONGLONG CompressedFileSize)
{
    FILE_COMPRESSION_INFO FileCompressionInfo;

    if (::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileCompressionInfo,
        &FileCompressionInfo,
        sizeof(FILE_COMPRESSION_INFO)))
    {
        *CompressedFileSize = static_cast<ULONGLONG>(
            FileCompressionInfo.CompressedFileSize.QuadPart);

        return TRUE;
    }

    return ::MileGetFileSizeByHandle(FileHandle, CompressedFileSize);
}

EXTERN_C BOOL WINAPI MileReadFile(
    _In_ HANDLE FileHandle,
    _Out_opt_ LPVOID Buffer,
    _In_ DWORD NumberOfBytesToRead,
    _Out_opt_ LPDWORD NumberOfBytesRead)
{
    BOOL Result = FALSE;
    DWORD NumberOfBytesTransferred = 0;
    OVERLAPPED Overlapped = { 0 };
    Overlapped.hEvent = ::CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr);
    if (Overlapped.hEvent)
    {
        Result = ::ReadFile(
            FileHandle,
            Buffer,
            NumberOfBytesToRead,
            &NumberOfBytesTransferred,
            &Overlapped);
        if (!Result)
        {
            if (ERROR_IO_PENDING == ::GetLastError())
            {
                Result = ::GetOverlappedResult(
                    FileHandle,
                    &Overlapped,
                    &NumberOfBytesTransferred,
                    TRUE);
            }
        }

        ::CloseHandle(Overlapped.hEvent);
    }
    else
    {
        ::SetLastError(ERROR_NO_SYSTEM_RESOURCES);
    }

    if (NumberOfBytesRead)
    {
        *NumberOfBytesRead = NumberOfBytesTransferred;
    }

    return Result;
}

EXTERN_C BOOL WINAPI MileWriteFile(
    _In_ HANDLE FileHandle,
    _In_opt_ LPCVOID Buffer,
    _In_ DWORD NumberOfBytesToWrite,
    _Out_opt_ LPDWORD NumberOfBytesWritten)
{
    BOOL Result = FALSE;
    DWORD NumberOfBytesTransferred = 0;
    OVERLAPPED Overlapped = { 0 };
    Overlapped.hEvent = ::CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr);
    if (Overlapped.hEvent)
    {
        Result = ::WriteFile(
            FileHandle,
            Buffer,
            NumberOfBytesToWrite,
            &NumberOfBytesTransferred,
            &Overlapped);
        if (!Result)
        {
            if (ERROR_IO_PENDING == ::GetLastError())
            {
                Result = ::GetOverlappedResult(
                    FileHandle,
                    &Overlapped,
                    &NumberOfBytesTransferred,
                    TRUE);
            }
        }

        ::CloseHandle(Overlapped.hEvent);
    }
    else
    {
        ::SetLastError(ERROR_NO_SYSTEM_RESOURCES);
    }

    if (NumberOfBytesWritten)
    {
        *NumberOfBytesWritten = NumberOfBytesTransferred;
    }

    return Result;
}

EXTERN_C BOOL WINAPI MileGetNtfsCompressionAttributeByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PUSHORT CompressionAlgorithm)
{
    if (!CompressionAlgorithm)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    DWORD BytesReturned = 0;
    return ::MileDeviceIoControl(
        FileHandle,
        FSCTL_GET_COMPRESSION,
        nullptr,
        0,
        CompressionAlgorithm,
        sizeof(*CompressionAlgorithm),
        &BytesReturned);
}

EXTERN_C BOOL WINAPI MileSetNtfsCompressionAttributeByHandle(
    _In_ HANDLE FileHandle,
    _In_ USHORT CompressionAlgorithm)
{
    switch (CompressionAlgorithm)
    {
    case COMPRESSION_FORMAT_NONE:
    case COMPRESSION_FORMAT_DEFAULT:
    case COMPRESSION_FORMAT_LZNT1:
        break;
    default:
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    DWORD BytesReturned = 0;
    return ::MileDeviceIoControl(
        FileHandle,
        FSCTL_SET_COMPRESSION,
        &CompressionAlgorithm,
        sizeof(CompressionAlgorithm),
        nullptr,
        0,
        &BytesReturned);
}

namespace
{
    typedef struct _WOF_FILE_PROVIDER_EXTERNAL_INFO
    {
        WOF_EXTERNAL_INFO Wof;
        FILE_PROVIDER_EXTERNAL_INFO FileProvider;
    } WOF_FILE_PROVIDER_EXTERNAL_INFO, * PWOF_FILE_PROVIDER_EXTERNAL_INFO;
}

EXTERN_C BOOL WINAPI MileGetWofFileCompressionAttributeByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PDWORD CompressionAlgorithm)
{
    if (!CompressionAlgorithm)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    WOF_FILE_PROVIDER_EXTERNAL_INFO WofInfo = { 0 };
    DWORD BytesReturned = 0;
    BOOL Result = ::MileDeviceIoControl(
        FileHandle,
        FSCTL_GET_EXTERNAL_BACKING,
        nullptr,
        0,
        &WofInfo,
        sizeof(WofInfo),
        &BytesReturned);
    if (Result)
    {
        if (WofInfo.Wof.Version == WOF_CURRENT_VERSION &&
            WofInfo.Wof.Provider == WOF_PROVIDER_FILE)
        {
            *CompressionAlgorithm = WofInfo.FileProvider.Algorithm;
        }
    }

    return Result;
}

EXTERN_C BOOL WINAPI MileSetWofFileCompressionAttributeByHandle(
    _In_ HANDLE FileHandle,
    _In_ DWORD CompressionAlgorithm)
{
    switch (CompressionAlgorithm)
    {
    case FILE_PROVIDER_COMPRESSION_XPRESS4K:
    case FILE_PROVIDER_COMPRESSION_LZX:
    case FILE_PROVIDER_COMPRESSION_XPRESS8K:
    case FILE_PROVIDER_COMPRESSION_XPRESS16K:
        break;
    default:
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    WOF_FILE_PROVIDER_EXTERNAL_INFO WofInfo = { 0 };
    WofInfo.Wof.Version = WOF_CURRENT_VERSION;
    WofInfo.Wof.Provider = WOF_PROVIDER_FILE;
    WofInfo.FileProvider.Version = FILE_PROVIDER_CURRENT_VERSION;
    WofInfo.FileProvider.Flags = 0;
    WofInfo.FileProvider.Algorithm = CompressionAlgorithm;
    DWORD BytesReturned = 0;
    BOOL Result = ::MileDeviceIoControl(
        FileHandle,
        FSCTL_SET_EXTERNAL_BACKING,
        &WofInfo,
        sizeof(WofInfo),
        nullptr,
        0,
        &BytesReturned);
    if (ERROR_COMPRESSION_NOT_BENEFICIAL == ::GetLastError() ||
        ERROR_MR_MID_NOT_FOUND == ::GetLastError())
    {
        Result = TRUE;
    }

    return Result;
}

EXTERN_C BOOL WINAPI MileRemoveWofFileCompressionAttributeByHandle(
    _In_ HANDLE FileHandle)
{
    DWORD BytesReturned = 0;
    BOOL Result = ::MileDeviceIoControl(
        FileHandle,
        FSCTL_DELETE_EXTERNAL_BACKING,
        nullptr,
        0,
        nullptr,
        0,
        &BytesReturned);
    if (ERROR_OBJECT_NOT_EXTERNALLY_BACKED == ::GetLastError())
    {
        Result = TRUE;
    }

    return Result;
}

EXTERN_C BOOL WINAPI MileLoadResource(
    _Out_ PMILE_RESOURCE_INFO ResourceInfo,
    _In_opt_ HMODULE ModuleHandle,
    _In_ LPCWSTR Type,
    _In_ LPCWSTR Name,
    _In_ WORD Language)
{
    if (!ResourceInfo)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    std::memset(ResourceInfo, 0, sizeof(MILE_RESOURCE_INFO));

    HRSRC ResourceFind = ::FindResourceExW(ModuleHandle, Type, Name, Language);
    if (!ResourceFind)
    {
        return FALSE;
    }

    ResourceInfo->Size = ::SizeofResource(ModuleHandle, ResourceFind);
    if (ResourceInfo->Size == 0)
    {
        return FALSE;
    }

    HGLOBAL ResourceLoad = ::LoadResource(ModuleHandle, ResourceFind);
    if (!ResourceLoad)
    {
        return FALSE;
    }

    ResourceInfo->Pointer = ::LockResource(ResourceLoad);

    return TRUE;
}

HANDLE MileCreateFile(
    _In_ LPCWSTR FileName,
    _In_ DWORD DesiredAccess,
    _In_ DWORD ShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES SecurityAttributes,
    _In_ DWORD CreationDisposition,
    _In_ DWORD FlagsAndAttributes,
    _In_opt_ HANDLE TemplateFile)
{
    return ::CreateFileW(
        Mile::FormatWideString(L"\\\\?\\%s", FileName).c_str(),
        DesiredAccess,
        ShareMode,
        SecurityAttributes,
        CreationDisposition,
        FlagsAndAttributes,
        TemplateFile);
}
