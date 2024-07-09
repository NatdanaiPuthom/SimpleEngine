#include "PinType.h"
#include "../DataType/DataTypeManager.h"
#include "../Global/ScriptGlobal.h"

namespace SCR
{


	std::string PinFlowTypeToString(const eFlowType aPinType)
	{
		switch (aPinType)
		{
		case eFlowType::Input:
			return "Input";
			break;
		case eFlowType::Output:
			return "Output";
			break;
		default:
			return "";
			break;
		}
	}
	eFlowType StringToPinFlowType(const std::string& aName)
	{
		if (aName == "Output")
		{
			return eFlowType::Output;
		}
		else if (aName == "Input")
		{
			return eFlowType::Input;
		}
		assert(false);
		return eFlowType::Input;
	}

	std::string GetPinLabel(const PinType& aPinType)
	{
		std::string pinLabel = aPinType.name;
		if (pinLabel == "#T")
		{
			pinLabel = Global::GetDataTypeManager().GetName(aPinType.dataTypeID);
		}
		return pinLabel;
	}
}