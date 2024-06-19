#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "ScriptTypeManager.h"

namespace SCR
{

	TypeManager::TypeManager()
	{
	}

	TypeManager::~TypeManager()
	{
	}

	DataTypeManager& TypeManager::GetDataTypeManager()
	{
		return myDataTypeManager;
	}

	NodeTypeManager& TypeManager::GetNodeTypeManager()
	{
		return myNodeTypeManager;
	}

	PinTypeManager& TypeManager::GetPinTypeManager()
	{
		return myPinTypeManager;
	}
}
