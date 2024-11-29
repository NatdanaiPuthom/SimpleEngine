#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeGraphFacade.hpp"
#include "FlyNodeFacade.hpp"

namespace FLY_NAMESPACE
{
	
	inline NodeFacade GetNodeOwner(PinFacade aPinFacade)
	{
		return NodeFacade(aPinFacade.GetNodeID(), NodeGraphFacade(aPinFacade.GetNodeGraphVariant()));
	}
}