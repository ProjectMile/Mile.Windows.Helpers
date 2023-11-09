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

std::wstring Mile::ToWideString(
    std::uint32_t CodePage,
    std::string_view const& InputString)
{
    std::wstring OutputString;

    int OutputStringLength = ::MultiByteToWideChar(
        CodePage,
        0,
        InputString.data(),
        static_cast<int>(InputString.size()),
        nullptr,
        0);
    if (OutputStringLength > 0)
    {
        OutputString.resize(OutputStringLength);
        OutputStringLength = ::MultiByteToWideChar(
            CodePage,
            0,
            InputString.data(),
            static_cast<int>(InputString.size()),
            &OutputString[0],
            OutputStringLength);
        OutputString.resize(OutputStringLength);
    }

    return OutputString;
}

std::string Mile::ToString(
    std::uint32_t CodePage,
    std::wstring_view const& InputString)
{
    std::string OutputString;

    int OutputStringLength = ::WideCharToMultiByte(
        CodePage,
        0,
        InputString.data(),
        static_cast<int>(InputString.size()),
        nullptr,
        0,
        nullptr,
        nullptr);
    if (OutputStringLength > 0)
    {
        OutputString.resize(OutputStringLength);
        OutputStringLength = ::WideCharToMultiByte(
            CodePage,
            0,
            InputString.data(),
            static_cast<int>(InputString.size()),
            &OutputString[0],
            OutputStringLength,
            nullptr,
            nullptr);
        OutputString.resize(OutputStringLength);
    }

    return OutputString;
}

namespace
{
    template <typename StringType>
    static std::vector<StringType> SplitCommandLineStringCommon(
        StringType const& CommandLine)
    {
        // Initialize the SplitArguments.
        std::vector<StringType> SplitArguments;

        typename StringType::value_type c = L'\0';
        int copy_character;                   /* 1 = copy char to *args */
        unsigned numslash;              /* num of backslashes seen */

        StringType Buffer;
        Buffer.reserve(CommandLine.size());

        /* first scan the program name, copy it, and count the bytes */
        typename StringType::value_type* p =
            const_cast<typename StringType::value_type*>(CommandLine.c_str());

        // A quoted program name is handled here. The handling is much simpler than
        // for other arguments. Basically, whatever lies between the leading
        // double-quote and next one, or a terminal null character is simply
        // accepted. Fancier handling is not required because the program name must
        // be a legal NTFS/HPFS file name. Note that the double-quote characters
        // are not copied, nor do they contribute to character_count.
        bool InQuotes = false;
        do
        {
            if (*p == '"')
            {
                InQuotes = !InQuotes;
                c = *p++;
                continue;
            }

            // Copy character into argument:
            Buffer.push_back(*p);

            c = *p++;
        } while (c != '\0' && (InQuotes || (c != ' ' && c != '\t')));

        if (c == '\0')
        {
            p--;
        }
        else
        {
            Buffer.resize(Buffer.size() - 1);
        }

        // Save te argument.
        SplitArguments.push_back(Buffer);

        InQuotes = false;

        // Loop on each argument
        for (;;)
        {
            if (*p)
            {
                while (*p == ' ' || *p == '\t')
                    ++p;
            }

            // End of arguments
            if (*p == '\0')
                break;

            // Initialize the argument buffer.
            Buffer.clear();

            // Loop through scanning one argument:
            for (;;)
            {
                copy_character = 1;

                // Rules:
                // 2N     backslashes + " ==> N backslashes and begin/end quote
                // 2N + 1 backslashes + " ==> N backslashes + literal "
                // N      backslashes     ==> N backslashes
                numslash = 0;

                while (*p == '\\')
                {
                    // Count number of backslashes for use below
                    ++p;
                    ++numslash;
                }

                if (*p == '"')
                {
                    // if 2N backslashes before, start/end quote, otherwise
                    // copy literally:
                    if (numslash % 2 == 0)
                    {
                        if (InQuotes && p[1] == '"')
                        {
                            p++; // Double quote inside quoted string
                        }
                        else
                        {
                            // Skip first quote char and copy second:
                            copy_character = 0; // Don't copy quote
                            InQuotes = !InQuotes;
                        }
                    }

                    numslash /= 2;
                }

                // Copy slashes:
                while (numslash--)
                {
                    Buffer.push_back(L'\\');
                }

                // If at end of arg, break loop:
                if (*p == '\0' || (!InQuotes && (*p == ' ' || *p == '\t')))
                    break;

                // Copy character into argument:
                if (copy_character)
                {
                    Buffer.push_back(*p);
                }

                ++p;
            }

            // Save te argument.
            SplitArguments.push_back(Buffer);
        }

        return SplitArguments;
    }
}

std::vector<std::wstring> Mile::SplitCommandLineWideString(
    std::wstring const& CommandLine)
{
    return ::SplitCommandLineStringCommon<std::wstring>(CommandLine);
}

std::vector<std::string> Mile::SplitCommandLineString(
    std::string const& CommandLine)
{
    return ::SplitCommandLineStringCommon<std::string>(CommandLine);
}
