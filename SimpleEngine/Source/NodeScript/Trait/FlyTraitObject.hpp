#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	struct TraitObject
	{
		NonOwningPtr<void> mDataPtr;
		DataTypeID mDataTypeID;
		TraitID mTraitID;
		size_t mFunctionIndex = InvalidID<size_t>();
	};

	
}