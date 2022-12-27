/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.Base.h
 * PURPOSE:   Definition for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
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

#endif // !MILE_WINDOWS_HELPERS_BASE
