/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.CppBase.cpp
 * PURPOSE:   Implementation for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mile.Helpers.CppBase.h"

#include <cstdarg>

std::wstring Mile::VFormatWideString(
    _In_z_ _Printf_format_string_ wchar_t const* const Format,
    _In_z_ _Printf_format_string_ va_list ArgList)
{
    // Check the argument list.
    if (Format)
    {
        // Get the length of the format result.
        std::size_t nLength =
            static_cast<std::size_t>(::_vscwprintf(Format, ArgList)) + 1;

        // Allocate for the format result.
        std::wstring Buffer(nLength + 1, L'\0');

        // Format the string.
        int nWritten = ::_vsnwprintf_s(
            &Buffer[0],
            Buffer.size(),
            nLength,
            Format,
            ArgList);

        if (nWritten > 0)
        {
            // If succeed, resize to fit and return result.
            Buffer.resize(nWritten);
            return Buffer;
        }
    }

    // If failed, return an empty string.
    return std::wstring();
}

std::wstring Mile::FormatWideString(
    _In_z_ _Printf_format_string_ wchar_t const* const Format,
    ...)
{
    va_list ArgList;
    va_start(ArgList, Format);
    std::wstring Result = Mile::VFormatWideString(Format, ArgList);
    va_end(ArgList);
    return Result;
}

std::string Mile::VFormatString(
    _In_z_ _Printf_format_string_ char const* const Format,
    _In_z_ _Printf_format_string_ va_list ArgList)
{
    // Check the argument list.
    if (Format)
    {
        // Get the length of the format result.
        std::size_t nLength =
            static_cast<std::size_t>(::_vscprintf(Format, ArgList)) + 1;

        // Allocate for the format result.
        std::string Buffer(nLength + 1, '\0');

        // Format the string.
        int nWritten = ::_vsnprintf_s(
            &Buffer[0],
            Buffer.size(),
            nLength,
            Format,
            ArgList);

        if (nWritten > 0)
        {
            // If succeed, resize to fit and return result.
            Buffer.resize(nWritten);
            return Buffer;
        }
    }

    // If failed, return an empty string.
    return std::string();
}

std::string Mile::FormatString(
    _In_z_ _Printf_format_string_ char const* const Format,
    ...)
{
    va_list ArgList;
    va_start(ArgList, Format);
    std::string Result = Mile::VFormatString(Format, ArgList);
    va_end(ArgList);
    return Result;
}
