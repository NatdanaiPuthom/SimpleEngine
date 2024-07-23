#include "FlyTypeManager.hpp"

namespace FLY_NAMESPACE
{

	TypeManager::TypeManager()
	{
	}

	TypeManager::~TypeManager()
	{
	}

	DataTypeManager& TypeManager::GetDataTypeManager()
	{
		return mDataTypeManager;
	}

	NodeTypeManager& TypeManager::GetNodeTypeManager()
	{
		return mNodeTypeManager;
	}

	PinTypeManager& TypeManager::GetPinTypeManager()
	{
		return mPinTypeManager;
	}
}
