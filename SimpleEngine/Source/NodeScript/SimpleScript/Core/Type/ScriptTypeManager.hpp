#pragma once
#include "../ScriptDefines.hpp"
#include "DataTypeManager.hpp"
#include "NodeTypeManager.hpp"
#include "PinTypeManager.hpp"

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