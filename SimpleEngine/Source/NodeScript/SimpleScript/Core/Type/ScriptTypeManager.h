#pragma once
#include "../ScriptDefines.h"
#include "DataTypeManager.h"
#include "NodeTypeManager.h"
#include "PinTypeManager.h"

namespace SCR
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

		DataTypeManager myDataTypeManager;
		NodeTypeManager myNodeTypeManager;
		PinTypeManager myPinTypeManager;
	};
}