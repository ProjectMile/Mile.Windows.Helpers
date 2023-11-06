/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.Base.h
 * PURPOSE:   Definition for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#pragma once

#ifndef MILE_WINDOWS_HELPERS_BASE
#define MILE_WINDOWS_HELPERS_BASE

#include <Windows.h>

/**
 * @brief Allocates a block of memory from the default heap of the calling
 *        process. The allocated memory will be initialized to zero. The
 *        allocated memory is not movable.
 * @param Size The number of bytes to be allocated.
 * @return If the function succeeds, the return value is a pointer to the
 *         allocated memory block. If the function fails, the return value is
 *         nullptr.
*/
EXTERN_C LPVOID WINAPI MileAllocateMemory(
    _In_ SIZE_T Size);

/**
 * @brief Reallocates a block of memory from the default heap of the calling
 *        process. If the reallocation request is for a larger size, the
 *        additional region of memory beyond the original size be initialized
 *        to zero. This function enables you to resize a memory block and
 *        change other memory block properties. The allocated memory is not
 *        movable.
 * @param Block A pointer to the block of memory that the function reallocates.
 *              This pointer is returned by an earlier call to
 *              MileAllocateMemory and MileReallocateMemory function.
 * @param Size The new size of the memory block, in bytes. A memory block's
 *             size can be increased or decreased by using this function.
 * @return If the function succeeds, the return value is a pointer to the
 *         reallocated memory block. If the function fails, the return value is
 *         nullptr.
*/
EXTERN_C LPVOID WINAPI MileReallocateMemory(
    _In_ PVOID Block,
    _In_ SIZE_T Size);

/**
 * @brief Frees a memory block allocated from a heap by the MileAllocateMemory
 *        and MileReallocateMemory function.
 * @param Block A pointer to the memory block to be freed. This pointer is
 *        returned by the Allocate or Reallocate method. If this pointer is
 *        nullptr, the behavior is undefined.
 * @return If the function succeeds, the return value is nonzero. If the
 *         function fails, the return value is zero. An application can call
 *         GetLastError for extended error information.
*/
EXTERN_C BOOL WINAPI MileFreeMemory(
    _In_ LPVOID Block);

/**
 * @brief Returns version information about the currently running operating
 *        system.
 * @param VersionInformation Pointer to either a OSVERSIONINFOW structure or a
 *                           OSVERSIONINFOEXW structure that contains the
 *                           version information about the currently running
 *                           operating system. A caller specifies which input
 *                           structure is used by setting the
 *                           dwOSVersionInfoSize member of the structure to the
 *                           size in bytes of the structure that is used.
 * @return TRUE if the function succeeds; otherwise, FALSE.
*/
EXTERN_C BOOL WINAPI MileGetWindowsVersion(
    _Inout_ LPOSVERSIONINFOW VersionInformation);

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
 * @brief Retrieves the number of milliseconds that have elapsed since the
 *        system was started.
 * @return The number of milliseconds.
*/
EXTERN_C ULONGLONG WINAPI MileGetTickCount();

/**
 * @brief Creates a thread to execute within the virtual address space of the
 *        calling process.
 * @param lpThreadAttributes A pointer to a SECURITY_ATTRIBUTES structure that
 *                           determines whether the returned handle can be
 *                           inherited by child processes.
 * @param dwStackSize The initial size of the stack, in bytes.
 * @param lpStartAddress A pointer to the application-defined function to be
 *                       executed by the thread.
 * @param lpParameter A pointer to a variable to be passed to the thread.
 * @param dwCreationFlags The flags that control the creation of the thread.
 * @param lpThreadId A pointer to a variable that receives the thread
 *                   identifier.
 * @return If the function succeeds, the return value is a handle to the new
 *         thread. If the function fails, the return value is nullptr. To get
 *         extended error information, call GetLastError.
 * @remark For more information, see CreateThread.
*/
EXTERN_C HANDLE WINAPI MileCreateThread(
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ SIZE_T dwStackSize,
    _In_ LPTHREAD_START_ROUTINE lpStartAddress,
    _In_opt_ LPVOID lpParameter,
    _In_ DWORD dwCreationFlags,
    _Out_opt_ LPDWORD lpThreadId);

/**
 * @brief Retrieves the number of logical processors in the current group.
 * @return The number of logical processors in the current group.
*/
EXTERN_C DWORD WINAPI MileGetNumberOfHardwareThreads();

/**
 * @brief Loads the specified module in the system directory into the address
 *        space of the calling process. The specified module may cause other
 *        modules to be loaded.
 * @param lpLibFileName The name of the module. This can be either a
 *                      library module (a .dll file) or an executable
 *                      module (an .exe file). The name specified is the
 *                      file name of the module and is not related to the
 *                      name stored in the library module itself, as
 *                      specified by the LIBRARY keyword in the
 *                      module-definition (.def) file.
 *                      If the string specifies a full path, the function
 *                      searches only that path for the module.
 *                      If the string specifies a relative path or a module
 *                      name without a path, the function uses a standard
 *                      search strategy to find the module.
 *                      If the function cannot find the module, the
 *                      function fails. When specifying a path, be sure to
 *                      use backslashes (\), not forward slashes (/).
 *                      If the string specifies a module name without a
 *                      path and the file name extension is omitted, the
 *                      function appends the default library extension .dll
 *                      to the module name. To prevent the function from
 *                      appending .dll to the module name, include a
 *                      trailing point character (.) in the module name
 *                      string.
 * @return If the function succeeds, the return value is a handle to the
 *         module. If the function fails, the return value is nullptr. To get
 *         extended error information, call GetLastError.
*/
EXTERN_C HMODULE WINAPI MileLoadLibraryFromSystem32(
    _In_ LPCWSTR lpLibFileName);

/**
 * @brief Indicates if the current process is elevated.
 * @return TRUE if the current process is elevated; otherwise, FALSE.
*/
EXTERN_C BOOL WINAPI MileIsCurrentProcessElevated();

#endif // !MILE_WINDOWS_HELPERS_BASE
