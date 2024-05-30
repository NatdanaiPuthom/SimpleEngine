//#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "InternalExecutionContext.h"
#include "ScriptProxy.h"

namespace SCR
{
	const Node& InternalExecutionContext::GetCurrentNode() const
	{
		return ScriptProxy::GetNode(script, nodeData.nodeID);
	}
}
