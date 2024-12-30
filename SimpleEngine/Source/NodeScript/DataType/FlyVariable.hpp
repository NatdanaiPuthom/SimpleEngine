#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
{
	struct Variable final
	{
		DataTypeID mDataTypeID = InvalidID<DataTypeID>();
		OwningPtr<void> mDefaultValueDataPtr;
		std::string mName = "Var";
		size_t mByteOffset = 0;
		bool mIsDestroyed = false;
	};

	struct DataTypeVariable final
	{
		DataTypeID mDataTypeID;
		OwningPtr<void> mDefaultValueDataPtr;
		std::string mName;
		size_t mByteOffset = 0;
	};
}