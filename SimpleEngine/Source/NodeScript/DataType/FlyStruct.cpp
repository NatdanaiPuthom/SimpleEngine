#include "FlyStruct.hpp"

namespace FLY_NAMESPACE
{

	Struct::Struct(std::string_view aName)
		: mName(aName)
	{
	}

	std::string& Struct::Name()
	{
		return mName;
	}

	const std::string& Struct::Name() const
	{
		return mName;
	}
}
