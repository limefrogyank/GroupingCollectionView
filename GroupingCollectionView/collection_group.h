#pragma once


namespace cx
{
	using namespace Windows::Foundation::Collections;
	using namespace Windows::UI::Xaml::Data;
}

namespace CustomCVS
{

	public ref class collection_group sealed :
		cx::ICollectionViewGroup
	{
	public:
		collection_group(Platform::Object^ group, cx::IObservableVector<Platform::Object^>^ groupItems);

		// Inherited via ICollectionViewGroup
		virtual property Platform::Object ^ Group
		{
			Platform::Object^ get() { return m_group; }
		}
		virtual property Windows::Foundation::Collections::IObservableVector<Platform::Object ^> ^ GroupItems
		{
			cx::IObservableVector<Platform::Object^>^ get() { return m_groupItems; }
		}

	private:
		Platform::Object^ m_group;
		cx::IObservableVector<Platform::Object^>^ m_groupItems;
	};

}

