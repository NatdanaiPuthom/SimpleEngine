#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeGraphProxy.hpp"
#include "FlyNodeProxy.hpp"

namespace FLY_NAMESPACE
{
	
	inline NodeProxy GetNodeOwner(PinProxy aPinProxy)
	{
		return NodeProxy(aPinProxy.GetNodeID(), NodeGraphProxy(aPinProxy.GetNodeGraphVariant()));
	}
}