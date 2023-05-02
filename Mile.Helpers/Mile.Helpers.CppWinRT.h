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
    
    /**
     * @brief A type that you can use to declare and implement a property of a
              specified type.
     * @tparam Type The type of property.
     * @remark https://devblogs.microsoft.com/oldnewthing/20230317-00/?p=107946.
    */
    template<typename Type>
    struct Property
    {
        Property(
            Type const& InitialValue = EmptyValue<Type>()) :
            Value(InitialValue)
        {
        }

        Type operator()() const
        {
            return this->Value;
        }

        void operator()(
            Type const& NewValue)
        {
            this->Value = NewValue;
        }

        Type Value;
    };
}

#endif // !MILE_WINDOWS_HELPERS_CPPWINRT
