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

#endif // !MILE_WINDOWS_HELPERS_BASE
