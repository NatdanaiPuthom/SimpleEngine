#include "PinType.hpp"
#include "../DataType/DataTypeManager.hpp"
#include "../Global/ScriptGlobal.hpp"

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
}