/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.CppBase.h
 * PURPOSE:   Definition for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#pragma once

#ifndef MILE_WINDOWS_HELPERS_CPPBASE
#define MILE_WINDOWS_HELPERS_CPPBASE

#include <string>

namespace Mile
{
    /**
     * @brief Write formatted data to a wide characters string, assumed UTF-16
     *        in Windows.
     * @param Format Format-control string.
     * @param ArgList Pointer to list of optional arguments to be formatted.
     * @return A formatted string if successful, an empty string otherwise.
    */
    std::wstring VFormatWideString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        _In_z_ _Printf_format_string_ va_list ArgList);

    /**
     * @brief Write formatted data to a wide characters string, assumed UTF-16
     *        in Windows.
     * @param Format Format-control string.
     * @param ... Optional arguments to be formatted.
     * @return A formatted string if successful, an empty string otherwise.
    */
    std::wstring FormatWideString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        ...);

    /**
     * @brief Write formatted data to a onebyte or multibyte string, suggested
     *        encoding with UTF-8.
     * @param Format Format-control string.
     * @param ArgList Pointer to list of optional arguments to be formatted.
     * @return A formatted string if successful, an empty string otherwise.
    */
    std::string VFormatString(
        _In_z_ _Printf_format_string_ char const* const Format,
        _In_z_ _Printf_format_string_ va_list ArgList);
}

#endif // !MILE_WINDOWS_HELPERS_CPPBASE
