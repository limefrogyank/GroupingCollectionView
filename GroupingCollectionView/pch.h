#pragma once

#include <unordered_map>
#include <collection.h>
#include <vector>

#include "winrt\Windows.Foundation.h"
#include <windows.foundation.h>

#include "winrt\Windows.ApplicationModel.Core.h"
#include <windows.applicationmodel.core.h>

#include <windows.ui.core.h>

#include "winrt\Windows.Foundation.Collections.h"
#include <windows.foundation.collections.h>

#include "winrt\Windows.UI.Xaml.h"

#include <windows.ui.xaml.interop.h>
#include "winrt\Windows.UI.Xaml.Interop.h"

#include <windows.ui.xaml.data.h>
#include "winrt\Windows.UI.Xaml.Data.h"


WINRT_WARNING_PUSH


template <typename T>
T^ to_cx(winrt::Windows::Foundation::IUnknown const& from)
{
	return safe_cast<T^>(reinterpret_cast<Platform::Object^>(winrt::get_abi(from)));
}

template <typename T>
T from_cx(Platform::Object^ from)
{
	T to{ nullptr };

	winrt::check_hresult(reinterpret_cast<::IUnknown*>(from)->QueryInterface(winrt::guid_of<T>(),
		reinterpret_cast<void**>(winrt::put_abi(to))));

	return to;
}