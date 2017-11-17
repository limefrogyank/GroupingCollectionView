#include "pch.h"
#include "GroupingCollectionView.h"

using namespace CustomCVS;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Interop;

bool CustomCVS::GroupingCollectionView::MoveCurrentToIndex(int i)
{
	if (i < -1 || i >= m_flattenedView.size())
	{
		return false;
	}

	if (i == m_index)
	{
		return false;
	}

	auto e = ref new cx::CurrentChangingEventArgs();
	CurrentChanging(this, e);
	if (e->Cancel)
	{
		return false;
	}

	m_index = i;
	CurrentChanged(this, nullptr);
	return true;
}

bool CustomCVS::GroupingCollectionView::MoveCurrentTo(Platform::Object ^item)
{
	if (item == this->CurrentItem)
		return true;
	else
	{
		unsigned int index;
		auto success = this->IndexOf(item, &index);
		if (success)
			return this->MoveCurrentToIndex(index);
		else
			return false;
	} 
}

bool CustomCVS::GroupingCollectionView::MoveCurrentToPosition(int index)
{
	return this->MoveCurrentToIndex(index);
}

bool CustomCVS::GroupingCollectionView::MoveCurrentToFirst()
{
	return this->MoveCurrentToIndex(0);
}

bool CustomCVS::GroupingCollectionView::MoveCurrentToLast()
{
	return this->MoveCurrentToIndex(m_flattenedView.size() - 1);
}

bool CustomCVS::GroupingCollectionView::MoveCurrentToNext()
{
	return this->MoveCurrentToIndex(m_index + 1);
}

bool CustomCVS::GroupingCollectionView::MoveCurrentToPrevious()
{
	return this->MoveCurrentToIndex(m_index - 1);
}

Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ CustomCVS::GroupingCollectionView::LoadMoreItemsAsync(unsigned int count)
{
	auto isil = dynamic_cast<cx::ISupportIncrementalLoading^>(Source);
	if (isil != nullptr)
		return isil->LoadMoreItemsAsync(count);
	else
		return to_cx<Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult>>(LoadMoreItemsAsyncWrapper(count));
}

winrt::IAsyncOperation<winrt::Windows::UI::Xaml::Data::LoadMoreItemsResult> CustomCVS::GroupingCollectionView::LoadMoreItemsAsyncWrapper(unsigned int count)
{
	co_await winrt::resume_background();

	LoadMoreItemsResult result;

	result.Count = 0;
	return result;
}
