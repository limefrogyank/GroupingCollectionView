#include "pch.h"
#include "collection_group.h"
#include "observable_vector.h"

using namespace CustomCVS;

collection_group::collection_group(Platform::Object^ group, cx::IObservableVector<Platform::Object^>^ groupItems)
	: m_group(group), m_groupItems(groupItems)
{

}
