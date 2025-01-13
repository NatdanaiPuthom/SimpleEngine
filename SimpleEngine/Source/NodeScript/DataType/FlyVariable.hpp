#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
{
	struct Variable final
	{
		GenericDataTypeID mDataTypeID;
		OwningPtr<void> mDefaultValueDataPtr;
		std::string mName = "Var";
		size_t mByteOffset = 0;
		bool mIsDestroyed = false;
	};
}