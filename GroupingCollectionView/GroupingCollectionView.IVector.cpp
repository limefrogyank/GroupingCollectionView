#include "pch.h"
#include "GroupingCollectionView.h"
#include "observable_vector.h"

using namespace CustomCVS;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Interop;


Windows::Foundation::Collections::IIterator<Platform::Object ^> ^ CustomCVS::GroupingCollectionView::First()
{
	std::vector<Platform::Object^> cxFlattenedView;
	for (auto & value : m_flattenedView)
		cxFlattenedView.push_back(to_cx<Platform::Object>(value));
	
	
	//auto casted = dynamic_cast<cx::IVector<Platform::Object^>^>(this);
	//auto stdVect = cx::to_vector(casted);
	auto v = ref new Platform::Collections::Vector<Platform::Object^>(cxFlattenedView);
	return v->First();
	//auto casted = safe_cast<cx::IObservableVector<Platform::Object^>^>(this);
	//auto it = ref new iterator<Platform::Object^>(casted);
	//return safe_cast<cx::IIterator<Platform::Object^>^>(it);
}