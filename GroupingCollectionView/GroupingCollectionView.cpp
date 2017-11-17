#include "pch.h"
#include "GroupingCollectionView.h"

#include "observable_vector.h"
#include "collection_group.h"


using namespace CustomCVS;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Interop;

namespace cx
{
	using namespace ::Windows::Foundation;
	using namespace ::Windows::UI::Xaml;
	using namespace ::Windows::UI::Xaml::Interop;
	using namespace ::Windows::UI::Xaml::Data;
	using namespace ::Windows::Foundation::Collections;
}

CustomCVS::GroupingCollectionView::GroupingCollectionView()
{	
	if (m_cxpropertyChangedTokens == nullptr)
		m_cxpropertyChangedTokens = ref new Platform::Collections::UnorderedMap<int, cx::EventRegistrationToken>();

	
	m_groupingSelector = [=](Platform::Object^ x) {
		//auto x2 = to_cx<Platform::Object>(x);
		auto y = GroupingSelector->Invoke(x);
		return y;
		//return from_cx<winrt::IInspectable>(y2);
	};
	
	//m_groupPropertyName = from_cx<winrt::hstring>(GroupingSelector);

	//m_itemSortFunction = [=](winrt::IInspectable const & x, winrt::IInspectable const & y) {
	//	auto x2 = to_cx<Platform::Object>(x);
	//	auto y2 = to_cx<Platform::Object>(y);
	//	int result = ItemSortFunction->Invoke(x2, y2);
	//	return result;
	//};
	m_itemSortFunction = [=](Platform::Object^ x, Platform::Object^ y) {
		int result = ItemSortFunction->Invoke(x, y);
		return result;
	};

	//m_source = from_cx<winrt::IInspectable>(Source);
	//m_groupingSelector = groupingSelector;
	//m_itemSortFunction = itemSortFunction;

	/*if (m_source != nullptr)
	{
		auto sourceNcc = m_source.try_as<INotifyCollectionChanged>();
		if (sourceNcc != nullptr)
		{
			m_collectionChangedToken = sourceNcc.CollectionChanged([=](auto &&, NotifyCollectionChangedEventArgs const& e)
			{
				this->SourceNcc_CollectionChanged(nullptr, e);
			});
		}

		AttachPropertyChangedHandler(m_source);
	}*/

	m_index = 0;
	

	/*auto test = "testing!";
	auto uri = winrt::Windows::Foundation::Uri(L"https://bing.com");
	IInspectable j = uri.try_as<IInspectable>();
	auto d = m_groupingSelector(j);*/
}

#pragma region Dependency Properties

cx::DependencyProperty^ GroupingCollectionView::m_GroupingSelector =
cx::DependencyProperty::Register("GroupingSelector",
	GroupingSelectorDelegate::typeid,
	GroupingCollectionView::typeid,
	ref new cx::PropertyMetadata(nullptr)
);

cx::DependencyProperty^ GroupingCollectionView::m_Source =
cx::DependencyProperty::Register("Source",
	cx::IBindableIterable::typeid,
	GroupingCollectionView::typeid,
	ref new cx::PropertyMetadata(nullptr, ref new cx::PropertyChangedCallback(&GroupingCollectionView::OnSourceChanged))
);
void CustomCVS::GroupingCollectionView::OnSourceChanged(cx::DependencyObject^ d, cx::DependencyPropertyChangedEventArgs^ e)
{
	GroupingCollectionView^ cvs = (GroupingCollectionView^)d;
	if (e->OldValue != nullptr)
	{
		auto incc = dynamic_cast<cx::INotifyCollectionChanged^>(e->OldValue);
		if (incc != nullptr)
			incc->CollectionChanged -= cvs->m_inccToken;
	}
	if (e->NewValue != nullptr)
	{
		auto incc = dynamic_cast<cx::INotifyCollectionChanged^>(e->NewValue);
		if (incc != nullptr)
			cvs->m_inccToken = incc->CollectionChanged += ref new Windows::UI::Xaml::Interop::NotifyCollectionChangedEventHandler(cvs,&CustomCVS::GroupingCollectionView::OnCollectionChanged);

	}
	//if (cvs->Source != nullptr)
	//	auto cc= cvs->CollectionGroups;
	cvs->HandleSourceChanged();
}

cx::DependencyProperty^ GroupingCollectionView::m_ItemSortFunction =
cx::DependencyProperty::Register("ItemSortFunction",
	ItemSortDelegate::typeid,
	GroupingCollectionView::typeid,
	ref new cx::PropertyMetadata(nullptr, ref new cx::PropertyChangedCallback(&GroupingCollectionView::OnItemSortFunctionChanged))
);
void CustomCVS::GroupingCollectionView::OnItemSortFunctionChanged(cx::DependencyObject^ d, cx::DependencyPropertyChangedEventArgs^ e)
{
	GroupingCollectionView^ cvs = (GroupingCollectionView^)d;
	ItemSortDelegate^ itemSortDelegate = dynamic_cast<ItemSortDelegate^>(e->NewValue);
	if (itemSortDelegate != nullptr)
	{
		cvs->m_itemSortFunction = [=](Platform::Object^ x, Platform::Object^ y) {
			int result = itemSortDelegate->Invoke(x, y);
			return result;
		};
	}
	cvs->HandleItemSortFunctionChanged();
}

#pragma endregion

void CustomCVS::GroupingCollectionView::HandleSourceChanged()
{
	auto currentItem = CurrentItem;
	m_flattenedView.clear();
	m_collectionGroups = GetCollectionGroups();

	for (int i=0;i<m_collectionGroups->Size;i++)
	{
		auto groupList = safe_cast<cx::ICollectionViewGroup^>(m_collectionGroups->GetAt(i));
		for (int j = 0; j < groupList->GroupItems->Size; j++)
		{
			auto item = groupList->GroupItems->GetAt(j);
			auto inpc = dynamic_cast<cx::INotifyPropertyChanged^>(item);
			if (inpc != nullptr)
			{
				auto token = inpc->PropertyChanged += ref new cx::PropertyChangedEventHandler([=](Platform::Object^, cx::IPropertyChangedEventArgs^ e)
				{
					this->ItemOnPropertyChanged(item, e);
				});
				m_cxpropertyChangedTokens->Insert(item->GetHashCode(), token);
			}
			m_flattenedView.push_back(from_cx<winrt::IInspectable>(item));
		}
	}
	
	VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
	MoveCurrentTo(currentItem);
}

void CustomCVS::GroupingCollectionView::HandleItemSortFunctionChanged()
{
	//resort items in each group
	if (m_collectionGroups != nullptr)
	{
		int count = 0;
		auto currentSelected = this->CurrentItem;
		//std::vector<winrt::IInspectable> oldFlattened(m_flattenedView);

		for (int i = 0; i<m_collectionGroups->Size; i++)
		{
			auto groupList = safe_cast<cx::ICollectionViewGroup^>(m_collectionGroups->GetAt(i));
			auto resortedVector = ref new observable_vector<Platform::Object^>();
			auto v = cx::to_vector(dynamic_cast<cx::IVector<Platform::Object^>^>(groupList->GroupItems));
		 	std::sort(
				v.begin(),v.end(),
				[=](Platform::Object^ x, Platform::Object^ y)
			{
				auto externalSortResult = ItemSortFunction(x, y);
				if (externalSortResult < 0)
					return true;
				else
					return false;
			});
			if (groupList->GroupItems->Size > 1)
			{
				//auto oldInner = cx::to_vector(groupList->GroupItems->GetView());
				groupList->GroupItems->Clear();
				//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
				std::vector<int> indexesChanged;
				for (int j = 0; j < v.size(); j++)
				{
					if (v[j] == currentSelected)
					{
						//CurrentChanging(this, ref new cx::CurrentChangingEventArgs());
						m_index = count;
					}
					groupList->GroupItems->Append(v[j]);
					/*int oldIndex = 0;
					for (int k = 0; k < oldFlattened.size(); k++)
					{
						if (to_cx<Platform::Object>(oldFlattened[k]) == v[j])
						{
							oldIndex = k;
							break;
						}
					}*/
					indexesChanged.push_back(count);
					//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemRemoved, oldIndex));
					//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemInserted, count));
					count++;
				}
				for (auto & ind : indexesChanged)
				{
					VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemChanged, ind));
				}
			}
			else
				count += groupList->GroupItems->Size;
		}
		m_flattenedView.clear();
		CreateFlattenedList(m_collectionGroups, m_flattenedView);
		//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
		CurrentChanged(this, nullptr);
	}
}

//void CustomCVS::GroupingCollectionView::SourceNcc_CollectionChanged(winrt::IInspectable const& sender, INotifyCollectionChangedEventArgs const& e)
//{
//	//switch (e.Action())
//	//{
//	//case NotifyCollectionChangedAction::Add:
//	//	AttachPropertyChangedHandler(e.NewItems().try_as<IBindableIterable>());
//	//	if (e.NewItems() != nullptr && e.NewItems().Size() == 1)
//	//	{
//	//		HandleItemAdded(e.NewStartingIndex(), e.NewItems().GetAt(0));
//	//	}
//	//	else
//	//	{
//	//		//HandleSourceChanged();
//	//	}
//
//	//	break;
//	//case NotifyCollectionChangedAction::Remove:
//	//	DetachPropertyChangedHandler(e.OldItems().try_as<IBindableIterable>());
//	//	if (e.OldItems() != nullptr && e.OldItems().Size() == 1)
//	//	{
//	//		//HandleItemRemoved(e.OldStartingIndex, e.OldItems[0]);
//	//	}
//	//	else
//	//	{
//	//		//HandleSourceChanged();
//	//	}
//
//	//	break;
//	//case NotifyCollectionChangedAction::Move:
//	//case NotifyCollectionChangedAction::Replace:
//	//case NotifyCollectionChangedAction::Reset:
//	//	//HandleSourceChanged();
//	//	break;
//	//}
//}
//
//void CustomCVS::GroupingCollectionView::AttachPropertyChangedHandler(IBindableIterable const& items)
//{
//	auto it = items.First();
//	while (it.MoveNext())
//	{
//		auto item = it.Current();
//		auto inpc = item.try_as<INotifyPropertyChanged>();
//		if (inpc != nullptr)
//		{
//			auto token = inpc.PropertyChanged([=](auto &&, IPropertyChangedEventArgs const& e)
//			{
//				this->ItemOnPropertyChanged(nullptr, e);
//			});
//			m_propertyChangedTokens.insert(std::pair<INotifyPropertyChanged, winrt::event_token>(inpc, token));
//		}
//	}
//}
//
//void CustomCVS::GroupingCollectionView::DetachPropertyChangedHandler(IBindableIterable const& items)
//{
//	auto it = items.First();
//	while (it.MoveNext())
//	{
//		auto item = it.Current();
//		auto inpc = item.try_as<INotifyPropertyChanged>();
//		if (inpc != nullptr)
//		{
//			auto token = m_propertyChangedTokens[inpc];
//			inpc.PropertyChanged(token);
//		}
//	}
//}
//
//void CustomCVS::GroupingCollectionView::ItemOnPropertyChanged(winrt::IInspectable const& item, IPropertyChangedEventArgs const& e)
//{
//
//}
//
//void  CustomCVS::GroupingCollectionView::HandleItemAdded(int sourceIndex, winrt::IInspectable const& item)
//{
//
//}


cx::IObservableVector<Platform::Object^>^ CustomCVS::GroupingCollectionView::GetCollectionGroups()
{
	observable_vector<Platform::Object^>^ outputGroups = ref new observable_vector<Platform::Object^>();

	if (Source != nullptr  && m_groupingSelector != nullptr)
	{
		//if (m_collectionGroups != nullptr)
		//	return m_collectionGroups;

		auto inputGroups = m_groupingSelector(Source);
		auto igrouping = safe_cast<cx::IIterable<cx::IBindableIterable^>^>(inputGroups);
		//auto igrouping = safe_cast<cx::IIterable<Platform::Object^>^>(groups);
		
		auto it = igrouping->First();
		int i = 0;
		do 
		{
			if (it->HasCurrent)
			{
				auto it2 = it->Current->First();
				int j = 0;
				std::vector<Platform::Object^> unsortedVector;
				auto internalVector = ref new observable_vector<Platform::Object^>();
				do
				{
					j++;
					if (it2->HasCurrent)
						unsortedVector.push_back(it2->Current);
						//internalVector->Append(it2->Current);
				} while (it2->MoveNext());

				std::sort(unsortedVector.begin(), unsortedVector.end(), [=](Platform::Object^ x, Platform::Object^ y) {
					auto externalSortResult = ItemSortFunction(x, y);
					if (externalSortResult < 0)
						return true;
					else
						return false;
				});
				for (auto & value: unsortedVector)
				{
					internalVector->Append(value);
				}

				auto cg = ref new collection_group(safe_cast<Platform::Object^>(it->Current), internalVector);
				outputGroups->Append(cg);
				i++;
			}
		} while (it->MoveNext());
		
		/*auto grouped = from(m_flattenedView).groupby([](winrt::IInspectable const & i) {
			return i;
		}).to_vector<>();
*/
	}
	auto collection = safe_cast<cx::IObservableVector<Platform::Object^>^>(outputGroups);

	return collection;

}






//GroupingCollectionView::~GroupingCollectionView()
//{
//}


void CustomCVS::GroupingCollectionView::OnCollectionChanged(Platform::Object ^sender, Windows::UI::Xaml::Interop::NotifyCollectionChangedEventArgs ^e)
{
	switch (e->Action)
	{
	case cx::NotifyCollectionChangedAction::Add:
		AttachPropertyChangedHandler(dynamic_cast<cx::IBindableIterable^>(e->NewItems));
		if (e->NewItems != nullptr && e->NewItems->Size == 1)
		{
			HandleItemAdded(e->NewStartingIndex, e->NewItems->GetAt(0));
		}
		else
		{
			//HandleSourceChanged();
		}

		break;
	case cx::NotifyCollectionChangedAction::Remove:
		DetachPropertyChangedHandler(dynamic_cast<cx::IBindableIterable^>(e->OldItems));
		if (e->OldItems != nullptr && e->OldItems->Size == 1)
		{
			HandleItemRemoved(e->OldStartingIndex, e->OldItems->GetAt(0));
		}
		else
		{
			//HandleSourceChanged();
		}

		break;
	case cx::NotifyCollectionChangedAction::Move:
	case cx::NotifyCollectionChangedAction::Replace:
	case cx::NotifyCollectionChangedAction::Reset:
		//HandleSourceChanged();
		break;
	}
}

void CustomCVS::GroupingCollectionView::AttachPropertyChangedHandler(cx::IBindableIterable^ items)
{
	auto it = items->First();
	do
	{
		auto item = it->Current;
		auto inpc = dynamic_cast<cx::INotifyPropertyChanged^>(item);
		if (inpc != nullptr)
		{
			auto token = inpc->PropertyChanged += ref new cx::PropertyChangedEventHandler([=](Platform::Object^, cx::IPropertyChangedEventArgs^ e)
			{
				this->ItemOnPropertyChanged(item, e);
			});
			m_cxpropertyChangedTokens->Insert(item->GetHashCode(), token);
		}
	} while (it->MoveNext());
}

void CustomCVS::GroupingCollectionView::DetachPropertyChangedHandler(cx::IBindableIterable^ items)
{
	auto it = items->First();
	do
	{
		auto item = it->Current;
		auto inpc = dynamic_cast<cx::INotifyPropertyChanged^>(item);
		if (inpc != nullptr)
		{
			auto token = m_cxpropertyChangedTokens->Lookup(item->GetHashCode());
			inpc->PropertyChanged -= token;
			m_cxpropertyChangedTokens->Remove(item->GetHashCode());
		}
	} while (it->MoveNext());
}

void CustomCVS::GroupingCollectionView::ItemOnPropertyChanged(Platform::Object^ item, cx::IPropertyChangedEventArgs^ e)
{

}

void CustomCVS::GroupingCollectionView::HandleItemAdded(int sourceIndex, Platform::Object^ item)
{
	auto newCollection = this->GetCollectionGroups();
	std::vector<winrt::IInspectable> newVector;
	CreateFlattenedList(newCollection, newVector);

	for (int i=0; i<newVector.size();i++)
	{
		if (newVector[i] == from_cx<winrt::IInspectable>(item))
		{
			//m_flattenedView = newVector;
			auto group = GetGroupContainingItem(newCollection, item);
			if (group->GroupItems->Size > 1)
			{
				for (int j = 0; j < group->GroupItems->Size; j++)
				{
					if (group->GroupItems->GetAt(j) == item)
					{
						auto existingGroup = GetGroupContainingSimilarItems(m_collectionGroups, group, item); //GetGroupContainingItem(m_collectionGroups, item);
						existingGroup->GroupItems->InsertAt(j, item);
						if (i < m_index)
							m_index++;
						this->InsertAt(i, item);
						//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemInserted, i));
					}
				}
			}
			else
			{
				//unsigned int gIndex = 0;
				unsigned int gIndex = GetIndexOfGroupContainingItem(newCollection, item);
				m_collectionGroups->InsertAt(gIndex, group);
				/*auto newGroup = ref new collection_group(group->Group, ref new observable_vector<Platform::Object^>());
				m_collectionGroups->InsertAt(gIndex, newGroup);
				newGroup->GroupItems->Append(item);*/
				if (i < m_index)
					m_index++;
				this->InsertAt(i, item);
				//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemInserted, i));
				//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
			}
		}
	}
}

void CustomCVS::GroupingCollectionView::HandleItemRemoved(int sourceIndex, Platform::Object^ item)
{
	/*auto newCollection = this->GetCollectionGroups();
	std::vector<winrt::IInspectable> newVector;
	CreateFlattenedList(newCollection, newVector);*/

	for (int i = 0; i<m_flattenedView.size(); i++)
	{
		if (m_flattenedView[i] == from_cx<winrt::IInspectable>(item))
		{
			auto group = GetGroupContainingItem(m_collectionGroups, item);
			if (group->GroupItems->Size > 1)
			{
				for (int j = 0; j < group->GroupItems->Size; j++)
				{
					if (group->GroupItems->GetAt(j) == item)
					{
						group->GroupItems->RemoveAt(j);
						if (i < m_index)
							m_index--;
						else if (i == m_index)
						{
							m_index = -1;
						}
						this->RemoveAt(i);
						//CreateFlattenedList(m_collectionGroups, m_flattenedView);
						//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemRemoved, i));
					}
				}
			}
			else
			{
				//last one in group, remove group
				//unsigned int gIndex = 0;
				unsigned int gIndex = GetIndexOfGroupContainingItem(m_collectionGroups, item);
				group->GroupItems->RemoveAt(0);
				
				if (i < m_index)
					m_index--;
				else if (i == m_index)
				{
					m_index = -1;
				}
				this->RemoveAt(i);
				m_collectionGroups->RemoveAt(gIndex);
				//CreateFlattenedList(m_collectionGroups, m_flattenedView);
				//VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemRemoved, i));
			}
		}
	}
	
}

void CustomCVS::GroupingCollectionView::CreateFlattenedList(cx::IObservableVector<Platform::Object^>^ groups, std::vector<winrt::IInspectable> & flattenedList)
{
	flattenedList.clear();
	for (int i = 0; i<groups->Size; i++)
	{
		auto groupList = safe_cast<cx::ICollectionViewGroup^>(groups->GetAt(i));
		for (int j = 0; j < groupList->GroupItems->Size; j++)
		{
			auto item = groupList->GroupItems->GetAt(j);
			/*auto inpc = dynamic_cast<cx::INotifyPropertyChanged^>(item);
			if (inpc != nullptr)
			{
				auto token = inpc->PropertyChanged += ref new cx::PropertyChangedEventHandler([=](Platform::Object^, cx::IPropertyChangedEventArgs^ e)
				{
					this->ItemOnPropertyChanged(item, e);
				});
				m_cxpropertyChangedTokens->Insert(item->GetHashCode(), token);
			}*/
			auto rtItem = from_cx<winrt::IInspectable>(item);

			flattenedList.push_back(rtItem);
		}
	}

}

cx::ICollectionViewGroup^ CustomCVS::GroupingCollectionView::GetGroupContainingItem(cx::IObservableVector<Platform::Object^>^ groups, Platform::Object^ item)
{
	auto it = groups->First();
	cx::ICollectionViewGroup^ foundGroup = nullptr;
	do
	{
		auto group = dynamic_cast<cx::ICollectionViewGroup^>(it->Current);
		auto it2 = group->GroupItems->First();
		do
		{
			if (it2->Current == item)
				foundGroup = group;
		} while (it2->MoveNext() && foundGroup == nullptr);
	} while (it->MoveNext() && foundGroup == nullptr);
	return foundGroup;
}

int CustomCVS::GroupingCollectionView::GetIndexOfGroupContainingItem(cx::IObservableVector<Platform::Object^>^ groups, Platform::Object^ item)
{
	int index = -1;
	auto it = groups->First();
	cx::ICollectionViewGroup^ foundGroup = nullptr;
	do
	{
		index++;
		auto group = dynamic_cast<cx::ICollectionViewGroup^>(it->Current);
		auto it2 = group->GroupItems->First();
		do
		{
			if (it2->Current == item)
				foundGroup = group;
		} while (it2->MoveNext() && foundGroup == nullptr);
	} while (it->MoveNext() && foundGroup == nullptr);
	return index;
}

cx::ICollectionViewGroup^ CustomCVS::GroupingCollectionView::GetGroupContainingSimilarItems(cx::IObservableVector<Platform::Object^>^ groups, cx::ICollectionViewGroup^ similarGroup, Platform::Object^ itemNotInFoundGroupYet)
{
	Platform::Object^ similarCommonItem = nullptr;
	for (int i = 0; i < similarGroup->GroupItems->Size; i++)
	{
		//get first item that is not the one not yet added
		auto potential = similarGroup->GroupItems->GetAt(i);
		if (potential != itemNotInFoundGroupYet)
		{
			similarCommonItem = potential;
			break;
		}
	}
	auto it = groups->First();
	cx::ICollectionViewGroup^ foundGroup = nullptr;
	do
	{
		auto group = dynamic_cast<cx::ICollectionViewGroup^>(it->Current);
		auto it2 = group->GroupItems->First();
		do
		{
			if (it2->Current == similarCommonItem)
				foundGroup = group;
		} while (it2->MoveNext() && foundGroup == nullptr);
	} while (it->MoveNext() && foundGroup == nullptr);
	return foundGroup;

}
