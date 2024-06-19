#include "ScriptGlobal.h"
#include "ScriptFoundation.h"
#include "DataType/DataTypeManager.h"
#include "NodeTypeManager.h"
#include "PinTypeManager.h"
#include "Type/ScriptTypeManager.h"

namespace SCR
{
	namespace Global
	{

		ScriptFoundation& GetFoundation()
		{
			return ScriptFoundation::GetInstance();
		}

		DataTypeManager& GetDataTypeManager()
		{
			return GetFoundation().GetTypeManager().GetDataTypeManager();
		}

		NodeTypeManager& GetNodeTypeManager()
		{
			return GetFoundation().GetTypeManager().GetNodeTypeManager();
		}

		PinTypeManager& GetPinTypeManager()
		{
			return GetFoundation().GetTypeManager().GetPinTypeManager();
		}
	}

}

