#include "pch.h"
#include "GroupingCollectionView.h"
#include "observable_vector.h"

using namespace CustomCVS;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Interop;



Platform::Object ^ CustomCVS::GroupingCollectionView::GetAt(unsigned int index)
{
	return to_cx<Platform::Object>(m_flattenedView[index]);
}

Windows::Foundation::Collections::IVectorView<Platform::Object ^> ^ CustomCVS::GroupingCollectionView::GetView()
{
	auto v = ref new Platform::Collections::Vector<Platform::Object^>();
	for (int i = 0; i < m_flattenedView.size(); i++)
	{
		v->Append(to_cx<Platform::Object>(m_flattenedView[i]));
	}
	//return m_collectionGroups->GetView();
	//for (int i = 0; i < m_flattenedView.size(); i++)
	//{
	//	v->Append(to_cx<Platform::Object>(m_flattenedView[i]));
	//}
	return v->GetView();
}

bool CustomCVS::GroupingCollectionView::IndexOf(Platform::Object ^value, unsigned int *index)
{
	bool success = false;
	for (unsigned int i = 0; i < m_flattenedView.size(); i++)
	{
		if (to_cx<Platform::Object>(m_flattenedView[i]) == value)
		{
			*index = i;
			success = true;
			break;
		}
	}
	return success;
}

void CustomCVS::GroupingCollectionView::SetAt(unsigned int index, Platform::Object ^value)
{
	m_flattenedView[index] = from_cx<winrt::IInspectable>(value);
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemChanged, index));
}

void CustomCVS::GroupingCollectionView::InsertAt(unsigned int index, Platform::Object ^value)
{
	m_flattenedView.insert(m_flattenedView.begin() + index, from_cx<winrt::IInspectable>(value));
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemInserted, index));
}

void CustomCVS::GroupingCollectionView::RemoveAt(unsigned int index)
{
	m_flattenedView.erase(m_flattenedView.begin() + index);
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemRemoved,index));
}

void CustomCVS::GroupingCollectionView::Append(Platform::Object ^value)
{
	m_flattenedView.push_back(from_cx<winrt::IInspectable>(value));
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemInserted, m_flattenedView.size()-1));
}

void CustomCVS::GroupingCollectionView::RemoveAtEnd()
{
	m_flattenedView.pop_back();
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemRemoved, m_flattenedView.size()));
}

void CustomCVS::GroupingCollectionView::Clear()
{
	m_flattenedView.clear();
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
}

unsigned int CustomCVS::GroupingCollectionView::GetMany(unsigned int startIndex, Platform::WriteOnlyArray<Platform::Object ^, 1U> ^items)
{
	if (startIndex >= m_flattenedView.size())
		return 0;
	auto actual = m_flattenedView.size() - startIndex;
	if (actual > items->Length)
		actual = items->Length;
	for (int i = startIndex; i < items->Length; i++)
		items[i] = to_cx<Platform::Object>(m_flattenedView[i]);
	return actual;
}

void CustomCVS::GroupingCollectionView::ReplaceAll(const Platform::Array<Platform::Object ^, 1U> ^items)
{
	m_flattenedView.clear();
	for (int i = 0; i < items->Length; i++)
		m_flattenedView.push_back(from_cx<winrt::IInspectable>(items->get(i)));
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
}
