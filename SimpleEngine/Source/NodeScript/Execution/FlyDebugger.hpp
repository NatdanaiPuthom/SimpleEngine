#pragma once
#include "../FlyDefines.hpp"
#include "FlyInternalExecutionContext.hpp"

namespace FLY_NAMESPACE
{
	struct NodeBreakpoint
	{
		NodeExecutionData mNodeExecutionData;

		explicit operator bool() const
		{
			return static_cast<bool>(mNodeExecutionData.mNodeRef);
		}
	};

	class Debugger final
	{
	public:
		Debugger() = default;



		void AddBreakpoint(const NodeRef& aNodeRef)
		{
			mBreakpoint.mNodeExecutionData.mNodeRef = aNodeRef;
		}

	private:

		NodeBreakpoint mBreakpoint;
	};
}