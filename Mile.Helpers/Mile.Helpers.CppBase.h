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

#include "Mile.Helpers.Base.h"

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

    /**
     * @brief Write formatted data to a onebyte or multibyte string, suggested
     *        encoding with UTF-8.
     * @param Format Format-control string.
     * @param ... Optional arguments to be formatted.
     * @return A formatted string if successful, an empty string otherwise.
    */
    std::string FormatString(
        _In_z_ _Printf_format_string_ char const* const Format,
        ...);

    /**
     * @brief Converts from the onebyte or multibyte string to the wide
     *        characters string.
     * @param CodePage Code page to use in performing the conversion. This
     *                 parameter can be set to the value of any code page
     *                 that is installed or available in the operating system.
     * @param InputString The onebyte or multibyte string you want to convert.
     * @return A converted wide characters string if successful, an empty
     *         string otherwise.
     * @remark For more information, see MultiByteToWideChar.
    */
    std::wstring ToWideString(
        std::uint32_t CodePage,
        std::string_view const& InputString);

    /**
     * @brief Converts from the wide characters string to the onebyte or
     *        multibyte string.
     * @param CodePage Code page to use in performing the conversion. This
     *                 parameter can be set to the value of any code page
     *                 that is installed or available in the operating system.
     * @param InputString The wide characters string you want to convert.
     * @return A converted onebyte or multibyte string if successful, an empty
     *         string otherwise.
     * @remark For more information, see WideCharToMultiByte.
    */
    std::string ToString(
        std::uint32_t CodePage,
        std::wstring_view const& InputString);

    /**
     * @brief Disables C++ class copy construction.
    */
    class DisableCopyConstruction
    {
    protected:
        DisableCopyConstruction() = default;
        ~DisableCopyConstruction() = default;

    private:
        DisableCopyConstruction(
            const DisableCopyConstruction&) = delete;
        DisableCopyConstruction& operator=(
            const DisableCopyConstruction&) = delete;
    };

    /**
     * @brief Disables C++ class move construction.
    */
    class DisableMoveConstruction
    {
    protected:
        DisableMoveConstruction() = default;
        ~DisableMoveConstruction() = default;

    private:
        DisableMoveConstruction(
            const DisableMoveConstruction&&) = delete;
        DisableMoveConstruction& operator=(
            const DisableMoveConstruction&&) = delete;
    };

    /**
     * @brief The template for defining the task when exit the scope.
     * @tparam TaskHandlerType The type of the task handler.
     * @remark For more information, see ScopeGuard.
    */
    template<typename TaskHandlerType>
    class ScopeExitTaskHandler :
        DisableCopyConstruction,
        DisableMoveConstruction
    {
    private:
        bool m_Canceled;
        TaskHandlerType m_TaskHandler;

    public:

        /**
         * @brief Creates the instance for the task when exit the scope.
         * @param EventHandler The instance of the task handler.
        */
        explicit ScopeExitTaskHandler(TaskHandlerType&& EventHandler) :
            m_Canceled(false),
            m_TaskHandler(std::forward<TaskHandlerType>(EventHandler))
        {

        }

        /**
         * @brief Executes and uninitializes the instance for the task when
         *        exit the scope.
        */
        ~ScopeExitTaskHandler()
        {
            if (!this->m_Canceled)
            {
                this->m_TaskHandler();
            }
        }

        /**
         * @brief Cancels the task when exit the scope.
        */
        void Cancel()
        {
            this->m_Canceled = true;
        }
    };
}

#endif // !MILE_WINDOWS_HELPERS_CPPBASE
