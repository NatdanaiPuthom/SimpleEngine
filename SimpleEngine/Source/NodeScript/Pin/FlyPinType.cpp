#include "FlyPinType.hpp"
#include <cassert>

namespace FLY_NAMESPACE
{
	PinType::PinType(std::string aName, GenericDataTypeID aDataTypeID, SetPinValueF aSetPinValueFunction, SetPinValueFromPinF aSetPinValueFromPinFunction, MemoryPoolID aDefaultValueMemoryID, eIODirection aIODirection)
		: mName(std::move(aName))
		, mGenericDataTypeID(std::move(aDataTypeID))
		, mSetPinValueFunction(aSetPinValueFunction)
		, mSetPinValueFromPinFunction(aSetPinValueFromPinFunction)
		, mDefaultValueID(aDefaultValueMemoryID)
		, mIODirection(aIODirection)
	{
	}

	const std::string& PinType::GetName() const
	{
		return mName;
	}

	void PinType::SetName(std::string aName)
	{
		mName = std::move(aName);
	}

	void PinType::AddSplitPinTypeID(PinTypeID aPinTypeID)
	{
		mSplitPinTypeIDs.push_back(aPinTypeID);
	}

	std::string PinIODirectionToString(const eIODirection aIODirection)
	{
		return SelectByIODirection(aIODirection, std::string("Input"), std::string("Output"));
	}

	eIODirection StringToPinIODirection(const std::string& aName)
	{
		if (aName == "Input")
		{
			return eIODirection::Input;
		}
		else if (aName == "Output")
		{
			return eIODirection::Output;
		}
		assert(false);
		return eIODirection::Input;
	}
}