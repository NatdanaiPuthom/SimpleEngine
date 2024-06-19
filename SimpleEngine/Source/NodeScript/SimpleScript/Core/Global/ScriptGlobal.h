#pragma once
#include "../ScriptDefines.h"

namespace SCR
{

	class ScriptFoundation;
	class DataTypeManager;
	class NodeTypeManager;
	class PinTypeManager;

	namespace Global
	{


		ScriptFoundation& GetFoundation();
		DataTypeManager& GetDataTypeManager();
		NodeTypeManager& GetNodeTypeManager();
		PinTypeManager& GetPinTypeManager();
	}


}