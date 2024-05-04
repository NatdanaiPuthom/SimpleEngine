#include "PinType.h"
#include "../DataType/DataTypeManager.h"

namespace SCR
{


	std::string PinFlowTypeToString(const ePinFlowType aPinType)
	{
		switch (aPinType)
		{
		case ePinFlowType::Input:
			return "Input";
			break;
		case ePinFlowType::Output:
			return "Output";
			break;
		default:
			return "";
			break;
		}
	}
	ePinFlowType StringToPinFlowType(const std::string& aName)
	{
		if (aName == "Output")
		{
			return ePinFlowType::Output;
		}
		else if (aName == "Input")
		{
			return ePinFlowType::Input;
		}
		return ePinFlowType::NotSet;
	}

	std::string GetPinLabel(const PinType& aPinType)
	{
		std::string pinLabel = aPinType.name;
		if (pinLabel == "#T")
		{
			pinLabel = DataTypeManager::GetName(aPinType.dataTypeID);
		}
		return pinLabel;
	}
}