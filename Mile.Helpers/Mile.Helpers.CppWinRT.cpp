/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.CppWinRT.cpp
 * PURPOSE:   Implementation for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mile.Helpers.CppWinRT.h"

#include <Windows.h>

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Resources.Core.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt
{
    using Windows::ApplicationModel::Package;
    using Windows::ApplicationModel::Resources::Core::ResourceManager;
    using Windows::ApplicationModel::Resources::Core::ResourceMap;
}

bool Mile::WinRT::IsPackagedMode()
{
    static bool CachedResult = ([]() -> bool
    {
        try
        {
            const auto CurrentPackage = winrt::Package::Current();
            return true;
        }
        catch (...)
        {
            return false;
        }
    }());

    return CachedResult;
}

winrt::hstring Mile::WinRT::GetLocalizedString(
    winrt::hstring const& ResourcePath,
    winrt::hstring const& FallbackString)
{
    try
    {
        winrt::ResourceMap CurrentResourceMap =
            winrt::ResourceManager::Current().MainResourceMap();

        if (CurrentResourceMap.HasKey(ResourcePath))
        {
            return CurrentResourceMap.Lookup(
                ResourcePath).Candidates().GetAt(0).ValueAsString();
        }
        else
        {
            return FallbackString;
        }
    }
    catch (...)
    {
        return FallbackString;
    }
}

winrt::hstring Mile::WinRT::GetLocalizedString(
    winrt::hstring const& ResourcePath)
{
    return Mile::WinRT::GetLocalizedString(ResourcePath, ResourcePath);
}

winrt::hresult_error Mile::WinRT::ToHResultError()
{
    try
    {
        throw;
    }
    catch (winrt::hresult_error const& ex)
    {
        return ex;
    }
    catch (std::bad_alloc const&)
    {
        return winrt::hresult_error(E_OUTOFMEMORY);
    }
    catch (std::out_of_range const& ex)
    {
        return winrt::hresult_out_of_bounds(winrt::to_hstring(ex.what()));
    }
    catch (std::invalid_argument const& ex)
    {
        return winrt::hresult_invalid_argument(winrt::to_hstring(ex.what()));
    }
    catch (std::exception const& ex)
    {
        return winrt::hresult_error(E_FAIL, winrt::to_hstring(ex.what()));
    }
}
