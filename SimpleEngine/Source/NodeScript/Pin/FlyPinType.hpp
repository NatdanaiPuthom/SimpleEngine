#pragma once
#include "../FlyDefines.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include "../Pin/FlyIODirection.hpp"
#include <string>
#include <vector>

namespace FLY_NAMESPACE
{
	class NodeGraph;

	struct SetPinValueData final
	{
		NodeGraph* mNodeGraph = nullptr;
		NodeGraphVariantHandle mNodeGraphVariantHandle;
		const void* mReadFromDataPtr = nullptr;
		PinID mWriteToPinID;
#ifdef FLY_DEBUG
		GenericDataTypeID mReadFromDataTypeID;
#endif
	};

	struct SetPinValueFromPinData final
	{
		NodeGraph* mWriteToPinNodeGraph = nullptr;
		const NodeGraph* mReadFromPinNodeGraph = nullptr;
		PinID mWriteToPinID;
		PinID mReadFromPinID;
	};

	class PinType final
	{
	public:

		PinType(std::string aName, GenericDataTypeID aDataTypeID, SetPinValueF aSetPinValueFunction, SetPinValueFromPinF aSetPinValueFromPinFunction, MemoryPoolID aDefaultValueMemoryID, eIODirection aIODirection);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;
		[[nodiscard]] SetPinValueF GetSetPinValueFunction() const;
		[[nodiscard]] SetPinValueFromPinF GetSetPinValueFromPinFunction() const;
		[[nodiscard]] MemoryPoolID GetDefaultValueMemoryID() const;
		[[nodiscard]] const std::vector<PinTypeID>& GetSplitPinTypeIDs() const;
		[[nodiscard]] eIODirection GetIODirection() const;
		
		void SetName(std::string aName);
		void AddSplitPinTypeID(PinTypeID aPinTypeID);

	private:

		std::string mName;
		//const DataTypeID mDataTypeID = InvalidID<DataTypeID>();
		GenericDataTypeID mGenericDataTypeID;
		SetPinValueF mSetPinValueFunction;
		SetPinValueFromPinF mSetPinValueFromPinFunction;
		MemoryPoolID mDefaultValueID = InvalidID<MemoryPoolID>();
		std::vector<PinTypeID> mSplitPinTypeIDs;
		eIODirection mIODirection = eIODirection::Input;
	};


	inline GenericDataTypeID PinType::GetDataTypeID() const
	{
		return mGenericDataTypeID;
	}

	inline SetPinValueF PinType::GetSetPinValueFunction() const
	{
		return mSetPinValueFunction;
	}

	inline SetPinValueFromPinF PinType::GetSetPinValueFromPinFunction() const
	{
		return mSetPinValueFromPinFunction;
	}

	inline MemoryPoolID PinType::GetDefaultValueMemoryID() const
	{
		return mDefaultValueID;
	}

	inline const std::vector<PinTypeID>& PinType::GetSplitPinTypeIDs() const
	{
		return mSplitPinTypeIDs;
	}

	inline eIODirection PinType::GetIODirection() const
	{
		return mIODirection;
	}

	[[nodiscard]] constexpr eIODirection InvertIODirection(const eIODirection aIODirection)
	{
		return aIODirection == eIODirection::Input ? eIODirection::Output : eIODirection::Input;
	}

	[[nodiscard]] std::string IODirectionToString(const eIODirection aIODirection);

	[[nodiscard]] eIODirection StringToIODirection(const std::string& aName);

	[[nodiscard]] constexpr decltype(auto) SelectByIODirection(eIODirection aIODirection, auto&& aInputValue, auto&& aOutputValue)
	{
		return aIODirection == eIODirection::Input ? aInputValue : aOutputValue;
	}

	[[nodiscard]] constexpr decltype(auto) SelectByIODirection(eIODirection aIODirection, const auto& aInputValue, const auto& aOutputValue)
	{
		return aIODirection == eIODirection::Input ? aInputValue : aOutputValue;
	}
}