/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.CppWinRT.h
 * PURPOSE:   Definition for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#ifndef MILE_WINDOWS_HELPERS_CPPWINRT
#define MILE_WINDOWS_HELPERS_CPPWINRT

#include <winrt/Windows.Foundation.h>

#include <type_traits>

namespace Mile::WinRT
{
    /**
     * @brief Helper to get the empty value.
     * @tparam Type The type of the empty value.
     * @return The empty value with a specified type.
     * @remark https://devblogs.microsoft.com/oldnewthing/20220504-00/?p=106569.
    */
    template<typename Type>
    constexpr Type EmptyValue() noexcept
    {
        if constexpr (std::is_base_of_v<
            winrt::Windows::Foundation::IUnknown,
            Type>)
        {
            return nullptr;
        }

        return {};
    }
    
}

#endif // !MILE_WINDOWS_HELPERS_CPPWINRT
