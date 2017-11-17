#include "pch.h"
#include "observable_vector.h"

using namespace CustomCVS;

//CustomCVS::observable_vector::observable_vector<T>()
//{
//}

//template<typename T>
//CustomCVS::observable_vector<T>::observable_vector()
//{
//	//throw ref new Platform::NotImplementedException();
//}

//template <typename T>
//T observable_vector::GetAt(unsigned int index)
//{
//	if (index >= m_values.size())
//		throw ref new Platform::Exception(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
//	return m_values[index];
//}

vector_changed_args::vector_changed_args(cx::CollectionChange collectionChange, unsigned int index)
	: m_collectionChange(collectionChange), m_index(index)
{
	
}

