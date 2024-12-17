#pragma once
#include "../FlyDefines.hpp"
#include <variant>

namespace FLY_NAMESPACE
{

	class EventGraph;
	class NodeGraph;
	
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

	using NodeGraphVariantHandle = std::variant<EventGraph*, FunctionIDWrapper>;

}