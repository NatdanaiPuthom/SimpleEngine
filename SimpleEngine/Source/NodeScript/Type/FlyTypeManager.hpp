#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataTypeManager.hpp"
#include "FlyNodeTypeManager.hpp"
#include "FlyPinTypeManager.hpp"

namespace FLY_NAMESPACE
{

	
	class TypeManager final
	{
	public:

		TypeManager();
		~TypeManager();

		
		DataTypeManager& GetDataTypeManager();
		NodeTypeManager& GetNodeTypeManager();
		PinTypeManager& GetPinTypeManager();

	private:

		DataTypeManager mDataTypeManager;
		NodeTypeManager mNodeTypeManager;
		PinTypeManager mPinTypeManager;
	};
}