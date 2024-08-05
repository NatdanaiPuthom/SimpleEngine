#pragma once
#include "../FlyDefines.hpp"
#include <variant>

namespace FLY_NAMESPACE
{

	class EventGraph;
	
	struct FunctionIDWrapper final
	{
		FunctionID mID = InvalidID<FunctionID>();
		
		inline friend bool operator==(const FunctionIDWrapper& a, const FunctionIDWrapper& b)
		{
			return a.mID == b.mID;
		}

		explicit operator bool() const
		{
			return mID != InvalidID<FunctionID>();
		}
	};

	using NodeGraphVariant = std::variant<EventGraph*, FunctionIDWrapper>;

}