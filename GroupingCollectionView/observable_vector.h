#pragma once

namespace cx
{
	using namespace Windows::Foundation::Collections;
	using namespace Windows::UI::Xaml::Data;
}

using namespace Windows::Foundation::Collections;

namespace CustomCVS
{

	template <typename T>
	ref class observable_vector sealed:
		cx::IObservableVector<T>
	{
	public:
		observable_vector<T>();

		// Inherited via IObservableVector
		//virtual event Windows::Foundation::Collections::VectorChangedEventHandler<T> ^ VectorChanged;
		virtual event Windows::Foundation::Collections::VectorChangedEventHandler<T> ^ VectorChanged
		{
			Windows::Foundation::EventRegistrationToken add(cx::VectorChangedEventHandler<T>^ handler)
			{
				return _event::add(handler);
			}
			void remove(Windows::Foundation::EventRegistrationToken token)
			{
				_event::remove(token);
			}
			void raise(cx::IObservableVector<T>^ sender, cx::IVectorChangedEventArgs^ e)
			{
				_event::raise(sender, e);
			}

		}
			
		
		// Inherited via IIterable
		virtual Windows::Foundation::Collections::IIterator<T> ^ First()
		{
			//auto it = ref new iterator<T>(this);
			//return safe_cast<cx::IIterator<T>^>(it);
			auto v = ref new Platform::Collections::Vector<T>(m_values);
			return v->First();
		}

		// Inherited via IVector
		virtual property unsigned int Size
		{
			unsigned int get() { return m_values.size(); }
		}
		virtual T GetAt(unsigned int index)
		{
			if (index >= m_values.size())
				throw ref new Platform::Exception(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
			return m_values[index];
		}
		virtual cx::IVectorView<T> ^ GetView()
		{
			auto v = ref new Platform::Collections::Vector<T>(m_values);
			//return safe_cast<cx::IVectorView<T>^>(this);
			return safe_cast<cx::IVectorView<T>^>(v->GetView());
		}
		virtual bool IndexOf(T value, unsigned int *index)
		{
			auto i = static_cast<uint32>(std::find(m_values.begin(), m_values.end(), value) - m_values.begin());
			index = &i;
			return i < m_values.size();
		}
		virtual void SetAt(unsigned int index, T value)
		{
			if (index >= m_values.size())
				throw ref new Platform::Exception(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
			++m_version;
			m_values[index] = value;
			//if (VectorChanged!= nullptr)
			VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemChanged, index));
		}
		virtual void InsertAt(unsigned int index, T value)
		{
			if (index > m_values.size())
				throw ref new Platform::Exception(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
			++m_version;
			m_values.insert(m_values.begin() + index, value);
			//if (VectorChanged != nullptr)
			VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemInserted, index));
		}
		virtual void RemoveAt(unsigned int index)
		{
			if (index > m_values.size())
				throw ref new Platform::Exception(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
			++m_version;
			m_values.erase(m_values.begin() + index);
			//if (VectorChanged != nullptr)
			VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemRemoved, index));
		}
		virtual void Append(T value)
		{
			++m_version;
			m_values.push_back(value);
			//if (VectorChanged != nullptr)
			VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemInserted, Size - 1));
		}
		virtual void RemoveAtEnd()
		{
			if (m_values.empty())
				throw ref new Platform::Exception(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
			++m_version;
			m_values.pop_back();
			//if (VectorChanged != nullptr)
			VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::ItemRemoved, Size));
		}
		virtual void Clear()
		{
			++m_version;
			m_values.clear();
			//if (VectorChanged != nullptr)
			VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
		}
		virtual unsigned int GetMany(unsigned int startIndex, Platform::WriteOnlyArray<T, 1U> ^items)
		{
			if (startIndex >= m_values.size())
				return 0;
			auto actual = m_values.size() - startIndex;
			if (actual > items->Length)
				actual = items->Length;
			for (int i = startIndex; i < items->Length; i++)
				items[i] = m_values[i];
			return actual;
		}
		virtual void ReplaceAll(const Platform::Array<T, 1U> ^items)
		{
			++m_version;
			m_values.assign(items->begin(), items->end());
			//if (VectorChanged != nullptr)
			VectorChanged(this, ref new vector_changed_args(cx::CollectionChange::Reset, 0));
		}

		cx::IVector<T>^ GetVector()
		{
			return ref new Platform::Collections::Vector<T>(m_values); 
		}
		property int GetVersion
		{
			int get() { return m_version; }
		}

	private:
		std::vector<Platform::Object^> m_values;
		//agile_event<VectorChangedEventHandler<T>> m_changed;
		int m_version;

		event cx::VectorChangedEventHandler<T>^ _event;

	};



	ref class vector_changed_args sealed :
		cx::IVectorChangedEventArgs
	{
	public:
		vector_changed_args(cx::CollectionChange collectionChange, unsigned int index);

		// Inherited via IVectorChangedEventArgs
		virtual property Windows::Foundation::Collections::CollectionChange CollectionChange
		{
			cx::CollectionChange get() { return m_collectionChange; }
		}
		virtual property unsigned int Index
		{
			unsigned int get() { return m_index; }
		}
	private:
		cx::CollectionChange m_collectionChange;
		unsigned int m_index;
	};


	template <typename T>
	ref class iterator sealed: cx::IIterator<T>
	{
	public:
		iterator(cx::IObservableVector<T>^ owner) 
			//m_version(owner->GetVersion)
			//m_current(owner->GetVector),
			//m_end(owner->m_values.end())
		{
			std::vector<T> sv = Windows::Foundation::Collections::to_vector<T>(owner->GetView());
			m_current = sv.begin();
			m_end = sv.end();
			m_owner = owner;//reinterpret_cast<Microsoft::WRL::ComPtr<observable_vector<T>^>>(owner);
		}

		
		virtual property T Current
		{
			T get() {
				if (m_current == m_end)
				{
					throw ref new Platform::Exception(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
				}

				return *m_current;
			}
		}

		virtual property bool HasCurrent
		{
			bool get() {
				return m_current != m_end;
			}
		}

		virtual bool MoveNext()
		{
			if (m_current != m_end)
			{
				++m_current;
			}

			return HasCurrent;
		}

		virtual unsigned int GetMany(Platform::WriteOnlyArray<T,1U>^ items)
		{
			uint32_t actual = static_cast<uint32_t>(std::distance(m_current, m_end));

			if (actual > items->Length)
			{
				actual = items->Length;
			}

			std::copy_n(m_current, actual, items->begin());
			std::advance(m_current, actual);
			return actual;
		}
	private:
		void abi_enter() const
		{
			if (m_version != m_owner->m_version)
			{
				throw hresult_changed_state();
			}
		}
	private:

		cx::IObservableVector<T>^ m_owner;
		//uint32_t const m_version;
		typename std::vector<T>::const_iterator m_current;
		typename std::vector<T>::const_iterator m_end;
	};



	template<typename T>
	inline observable_vector<T>::observable_vector()
	{
		
	}

}