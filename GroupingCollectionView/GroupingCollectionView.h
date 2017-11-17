#pragma once

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Interop;
using namespace winrt::Windows::UI::Xaml::Data;

namespace cx
{
	using namespace ::Windows::Foundation;
	using namespace ::Windows::UI::Xaml;
	using namespace ::Windows::UI::Xaml::Interop;
	using namespace ::Windows::UI::Xaml::Data;
	using namespace ::Windows::Foundation::Collections;
}

namespace winrt
{
	using namespace Windows::Foundation;
	using namespace Windows::UI::Xaml;
}

namespace CustomCVS
{
	public delegate Platform::Object^ GroupingSelectorDelegate(Platform::Object^ obj);
	public delegate int ItemSortDelegate(Platform::Object^ a, Platform::Object^ b);

	public ref class GroupingCollectionView sealed : public cx::DependencyObject, public cx::ICollectionView
	{
	public:
		GroupingCollectionView();
		//~GroupingCollectionView();

#pragma region DependencyProperties
		static property cx::DependencyProperty^ GroupingSelectorProperty
		{
			cx::DependencyProperty^ get()
			{
				return m_GroupingSelector;
			}
		}
		property GroupingSelectorDelegate^ GroupingSelector
		{
			GroupingSelectorDelegate^ get()
			{
				return (GroupingSelectorDelegate^)GetValue(GroupingSelectorProperty);
			}
			void set(GroupingSelectorDelegate^ value)
			{
				SetValue(GroupingSelectorProperty, value);
			}
		}


		static property cx::DependencyProperty^ SourceProperty
		{
			cx::DependencyProperty^ get()
			{
				return m_Source;
			}
		}
		property cx::IBindableIterable ^ Source
		{
			cx::IBindableIterable^ get()
			{
				return (cx::IBindableIterable^)GetValue(SourceProperty);
			}
			void set(cx::IBindableIterable^ value)
			{
				SetValue(SourceProperty, value);
			}
		}


		static property cx::DependencyProperty^ ItemSortFunctionProperty
		{
			cx::DependencyProperty^ get()
			{
				return m_ItemSortFunction;
			}
		}
		property ItemSortDelegate^ ItemSortFunction
		{
			ItemSortDelegate^ get()
			{
				return (ItemSortDelegate^)GetValue(ItemSortFunctionProperty);
			}
			void set(ItemSortDelegate^ value)
			{
				SetValue(ItemSortFunctionProperty, value);
			}
		}
#pragma endregion

		// Inherited via IVector
		virtual Windows::Foundation::Collections::IIterator<Platform::Object ^> ^ First();

		// Inherited via IObservableVector
		virtual property unsigned int Size
		{
			unsigned int get()
			{
				return m_flattenedView.size();
			}
		}
		virtual Platform::Object ^ GetAt(unsigned int index);
		virtual Windows::Foundation::Collections::IVectorView<Platform::Object ^> ^ GetView();
		virtual bool IndexOf(Platform::Object ^value, unsigned int *index);
		virtual void SetAt(unsigned int index, Platform::Object ^value);
		virtual void InsertAt(unsigned int index, Platform::Object ^value);
		virtual void RemoveAt(unsigned int index);
		virtual void Append(Platform::Object ^value);
		virtual void RemoveAtEnd();
		virtual void Clear();
		virtual unsigned int GetMany(unsigned int startIndex, Platform::WriteOnlyArray<Platform::Object ^, 1U> ^items);
		virtual void ReplaceAll(const Platform::Array<Platform::Object ^, 1U> ^items);

		// Inherited via ICollectionView
		virtual event Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object ^> ^ VectorChanged;
		virtual property Windows::Foundation::Collections::IObservableVector<Platform::Object ^> ^ CollectionGroups
		{
			cx::IObservableVector<Platform::Object^>^ get() { return m_collectionGroups; }
		}
		virtual property Platform::Object ^ CurrentItem
		{
			Platform::Object^ get() 
			{ 
				if (m_index >= 0 && m_index < m_flattenedView.size())
				{
					auto obj = to_cx<Platform::Object>(m_flattenedView[m_index]);
					return obj;
				}
				else
					return nullptr;
			}
		}
		virtual property int CurrentPosition
		{
			int get()
			{
				return m_index;
			}
		}
		virtual property bool HasMoreItems
		{
			bool get() 
			{ 
				if (Source == nullptr)
					return false;
				auto isil = dynamic_cast<cx::ISupportIncrementalLoading^>(Source);
				if (isil != nullptr)return isil->HasMoreItems; else return false;
			}
		}
		virtual property bool IsCurrentAfterLast
		{
			bool get() { return m_index >= m_flattenedView.size(); }
		}
		virtual property bool IsCurrentBeforeFirst
		{
			bool get() { return m_index < 0; }
		}
		virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^ CurrentChanged;
		virtual event Windows::UI::Xaml::Data::CurrentChangingEventHandler ^ CurrentChanging;
		virtual bool MoveCurrentTo(Platform::Object ^item);
		virtual bool MoveCurrentToPosition(int index);
		virtual bool MoveCurrentToFirst();
		virtual bool MoveCurrentToLast();
		virtual bool MoveCurrentToNext();
		virtual bool MoveCurrentToPrevious();
		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count);

	private:
		static cx::DependencyProperty^ m_GroupingSelector;
		static cx::DependencyProperty^ m_Source;
		static cx::DependencyProperty^ m_ItemSortFunction;

		static void OnSourceChanged(cx::DependencyObject^ d, cx::DependencyPropertyChangedEventArgs^ e);
		static void OnItemSortFunctionChanged(cx::DependencyObject^ d, cx::DependencyPropertyChangedEventArgs^ e);

		cx::EventRegistrationToken m_inccToken;
		//IBindableIterable m_source = nullptr;
		std::function<Platform::Object^(Platform::Object^)> m_groupingSelector;
		//winrt::hstring m_groupPropertyName;
		//std::function<int(winrt::IInspectable const&, winrt::IInspectable const&)> m_itemSortFunction;
		std::function<int(Platform::Object^, Platform::Object^)> m_itemSortFunction;

		cx::IObservableVector<Platform::Object^>^ m_collectionGroups;

		unsigned int m_index;
		std::vector<winrt::IInspectable> m_flattenedView;

		/*void SourceNcc_CollectionChanged(winrt::IInspectable const& sender, INotifyCollectionChangedEventArgs const& e);
		winrt::event_token m_collectionChangedToken;
*/
		/*void HandleItemAdded(int sourceIndex, winrt::IInspectable const& item);
		void HandleSourceChanged();*/

		/*void AttachPropertyChangedHandler(IBindableIterable const& items);
		void DetachPropertyChangedHandler(IBindableIterable const& items);
		std::unordered_map<INotifyPropertyChanged, winrt::event_token> m_propertyChangedTokens;
		void ItemOnPropertyChanged(winrt::IInspectable const& item, IPropertyChangedEventArgs const& e);
*/
		void AttachPropertyChangedHandler(cx::IBindableIterable^ items);
		void DetachPropertyChangedHandler(cx::IBindableIterable^ items);
		Platform::Collections::UnorderedMap<int, cx::EventRegistrationToken>^ m_cxpropertyChangedTokens;
		
		void ItemOnPropertyChanged(Platform::Object^ object, cx::IPropertyChangedEventArgs^ e);

		void HandleItemAdded(int sourceIndex, Platform::Object^ item);
		void HandleItemRemoved(int sourceIndex, Platform::Object^ item);
		void HandleSourceChanged();
		void HandleItemSortFunctionChanged();

		cx::IObservableVector<Platform::Object^>^ GetCollectionGroups();
		void CreateFlattenedList(cx::IObservableVector<Platform::Object^>^ groups, std::vector<winrt::IInspectable> & flattenedList);
		cx::ICollectionViewGroup^ GetGroupContainingItem(cx::IObservableVector<Platform::Object^>^ groups, Platform::Object^ item);
		int GetIndexOfGroupContainingItem(cx::IObservableVector<Platform::Object^>^ groups, Platform::Object^ item);
		cx::ICollectionViewGroup^ GetGroupContainingSimilarItems(cx::IObservableVector<Platform::Object^>^ groups, cx::ICollectionViewGroup^ similarGroup, Platform::Object^ itemNotInFoundGroupYet);


		winrt::IAsyncOperation<winrt::Windows::UI::Xaml::Data::LoadMoreItemsResult> LoadMoreItemsAsyncWrapper(unsigned int count);

		bool MoveCurrentToIndex(int i);

		void OnCollectionChanged(Platform::Object ^sender, Windows::UI::Xaml::Interop::NotifyCollectionChangedEventArgs ^e);
};

}