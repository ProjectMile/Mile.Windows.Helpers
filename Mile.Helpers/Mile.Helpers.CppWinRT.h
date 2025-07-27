/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Helpers.CppWinRT.h
 * PURPOSE:    Definition for the essential Windows helper functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
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

    /**
     * @brief A type that you can use to declare and implement an event of a
     *        specified delegate type.
     * @tparam Delegate The type of delegate that can register to handle the
     *                  event.
     * @remark https://devblogs.microsoft.com/oldnewthing/20230317-00/?p=107946.
    */
    template<typename Delegate>
    struct Event : winrt::event<Delegate>
    {
        Event() = default;

        using winrt::event<Delegate>::operator();

        auto operator()(
            Delegate const& Handler)
        {
            return this->add(Handler);
        }

        void operator()(
            winrt::event_token const& Token)
        {
            this->remove(Token);
        }
    };

    /**
     * @brief Indicates if the current environment is packaged mode.
     * @return Return true if the current environment is packaged mode.
    */
    bool IsPackagedMode();

    /**
     * @brief Retrieves the localized string from Package Resource Index.
     * @param ResourcePath The resource path in Package Resource Index.
     * @param FallbackString The fallback string when operation failed.
     * @return The localized string from Package Resource Index.
    */
    winrt::hstring GetLocalizedString(
        winrt::hstring const& ResourcePath,
        winrt::hstring const& FallbackString);

    /**
     * @brief Retrieves the localized string from Package Resource Index.
     * @param ResourcePath The resource path in Package Resource Index.
     * @return The localized string from Package Resource Index.
    */
    winrt::hstring GetLocalizedString(
        winrt::hstring const& ResourcePath);

    /**
     * @brief A helper function, for use in a catch block, that turns the last
     *        exception thrown into winrt::hresult_error if the exception that
     *        was thrown is any of: winrt::hresult_error, std::bad_alloc,
     *        std::out_of_range, std::invalid_argument, or std::exception.
     * @return The winrt::hresult_error struct represented by the last exception
     *         thrown.
    */
    winrt::hresult_error ToHResultError();
}

#endif // !MILE_WINDOWS_HELPERS_CPPWINRT
