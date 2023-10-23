/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Helpers.CppWinRT.cpp
 * PURPOSE:   Implementation for the essential Windows helper functions
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#include "Mile.Helpers.CppWinRT.h"

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
