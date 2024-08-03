#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	struct Variable final
	{
		DataTypeID mDataTypeID = InvalidID<DataTypeID>();
		OwningPtr<void> mDefaultValueDataPtr = nullptr;
		std::string mName = "Var";
		size_t mByteOffset = 0;
		bool mIsDestroyed = false;
	};
}