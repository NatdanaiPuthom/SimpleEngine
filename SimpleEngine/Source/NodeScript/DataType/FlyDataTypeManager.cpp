#include "FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeManager::DataTypeManager()
	{
	}

	DataTypeManager::~DataTypeManager()
	{
	}

	EditAndViewResult DataTypeManager::ViewAndEditData(const DataType& aDataType, void* const aDataPtr) const
	{
		// If data type has a valid edit function
		if (aDataType.mInterface.function.viewAndEdit)
		{
			return aDataType.mInterface.function.viewAndEdit(aDataPtr);
		}

		EditAndViewResult editAndViewResult;
		// View and edit member variables instead
		for (const Variable& variable : aDataType.mVariables)
		{
			if (const DataType* variableDataType = Find(variable.mDataTypeID))
			{
				void* const propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + variable.mByteOffset);
				editAndViewResult.mIsItemActive |= ViewAndEditData(*variableDataType, propertyDataPtr).mIsItemActive;
			}
		}

		return editAndViewResult;
	}

	void DataTypeManager::ViewData(const DataType& aDataType, const void* const aDataPtr) const
	{
		// If data type has a valid edit function
		if (aDataType.mInterface.function.view)
		{
			aDataType.mInterface.function.view(aDataPtr);
			return;
		}

		// View member variables instead
		for (const Variable& variable : aDataType.mVariables)
		{
			if (const DataType* variableDataType = Find(variable.mDataTypeID))
			{
				const void* const propertyDataPtr = reinterpret_cast<const void*>(reinterpret_cast<size_t>(aDataPtr) + variable.mByteOffset);
				ViewData(*variableDataType, propertyDataPtr);
			}
		}

	}

	bool DataTypeManager::SaveData(const DataType& aDataType, nlohmann::json& aJson, const void* aDataPtr) const
	{
		// If data type has a valid save function
		if (aDataType.mInterface.function.save)
		{
			aDataType.mInterface.function.save(aJson, aDataPtr);
			return true;
		}

		// Save member variables instead
		for (const Variable& variable : aDataType.mVariables)
		{
			if (const DataType* variableDataType = Find(variable.mDataTypeID))
			{
				nlohmann::json variableJson;
				const void* variableDataPtr = reinterpret_cast<const void*>(reinterpret_cast<size_t>(aDataPtr) + variable.mByteOffset);
				if (SaveData(*variableDataType, variableJson, variableDataPtr))
				{
					aJson[variable.mName] = variableJson;
				}
			}
		}

		return false;
	}

	bool DataTypeManager::LoadData(const DataType& aDataType, const nlohmann::json& aJson, void* const aDataPtr) const
	{
		// If data type has a valid load function
		if (aDataType.mInterface.function.load)
		{
			aDataType.mInterface.function.load(aJson, aDataPtr);
			return true;
		}

		// Load member variables instead
		for (const Variable& variable : aDataType.mVariables)
		{
			if (const DataType* dataType = Find(variable.mDataTypeID))
			{
				auto it = aJson.find(variable.mName);
				if (it != aJson.end())
				{
					void* propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + variable.mByteOffset);
					LoadData(*dataType, it.value(), propertyDataPtr);
				}
			}
		}

		return false;
	}

	EditAndViewResult DataTypeManager::ViewAndEditData(const DataTypeID aDataTypeID, void* const aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return ViewAndEditData(*dataType, aDataPtr);
		}

		return EditAndViewResult{ .mIsItemActive = false };
	}

	void DataTypeManager::ViewData(DataTypeID aDataTypeID, const void* aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			ViewData(*dataType, aDataPtr);
		}
	}

	bool DataTypeManager::SaveData(const DataTypeID aDataTypeID, nlohmann::json& aJson, const void* const aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SaveData(*dataType, aJson, aDataPtr);
		}
		return false;
	}

	bool DataTypeManager::LoadData(const DataTypeID aDataTypeID, const nlohmann::json& aJson, void* const aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return LoadData(*dataType, aJson, aDataPtr);
		}
		return false;
	}

	void DataTypeManager::ReleaseData(const DataTypeID aDataTypeID, void* const aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.fundamental.release)
			{
				dataType->mInterface.fundamental.release(aDataPtr);
			}
		}
	}

	void DataTypeManager::CopyData(const DataTypeID aDataTypeID, void* const aDestination, const void* const aSource) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.fundamental.copy)
			{
				dataType->mInterface.fundamental.copy(aDestination, aSource);
			}
		}
	}

	void DataTypeManager::SwapData(const DataTypeID aDataTypeID, void* const aDataPtr1, void* const aDataPtr2) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.fundamental.swap)
			{
				dataType->mInterface.fundamental.swap(aDataPtr1, aDataPtr2);
			}
		}
	}

	bool DataTypeManager::DataEqualsTo(const DataTypeID aDataTypeID, const void* const aDataPtr1, const void* const aDataPtr2) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.fundamental.equals)
			{
				return dataType->mInterface.fundamental.equals(aDataPtr1, aDataPtr2);
			}
		}
		return false;
	}

	bool DataTypeManager::AreDataTypesRelated(DataTypeID aDataTypeID1, DataTypeID aDataTypeID2) const
	{
		if (aDataTypeID1 == aDataTypeID2)
		{
			return true;
		}
		auto checker = [this](DataTypeID a, DataTypeID b) -> bool
			{
				if (const DataType* dataType = Find(a))
				{
					return dataType->mToPointerDataTypeID == b || dataType->mToValueDataTypeID == b;
				}
				return false;
			};

		return checker(aDataTypeID1, aDataTypeID2) || checker(aDataTypeID2, aDataTypeID1);
	}

	const std::string& DataTypeManager::GetName(const DataTypeID aDataTypeID) const
	{
		if (mDataTypes.contains(aDataTypeID))
		{
			return mDataTypes.at(aDataTypeID).mName;
		}
		return mNullNameStr;
	}

	SetPinValueInterface DataTypeManager::GetSetPinValueInterface(const DataTypeID aDataTypeID, const eFlowType aFlowType) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SelectByFlowType(aFlowType, dataType->mInterface.execution.setInputPinValue, dataType->mInterface.execution.setOutputPinValue);
		}

		assert(false);
		return nullptr;
	}

	SetPinValueFromPinInterface DataTypeManager::GetSetPinValueFromPinInterface(const DataTypeID aDataTypeID, const eFlowType aFlowType) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SelectByFlowType(aFlowType, dataType->mInterface.execution.setInputPinValueFromPin, dataType->mInterface.execution.setOutputPinValueFromPin);
		}

		assert(false);
		return nullptr;
	}

	DataTypeID DataTypeManager::GetDataTypeIDByName(const std::string& aName) const
	{
		for (const auto& [dataTypeID, dataType] : mDataTypes)
		{
			if (aName == dataType.mName)
			{
				return dataTypeID;
			}
		}
		return InvalidID<DataTypeID>();
	}

	const std::unordered_map<DataTypeID, DataType>& DataTypeManager::GetDataTypes() const
	{
		return mDataTypes;
	}


	DataType* DataTypeManager::Find(const DataTypeID aDataTypeID)
	{
		auto it = mDataTypes.find(aDataTypeID);
		if (it != mDataTypes.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	const DataType* DataTypeManager::Find(DataTypeID aDataTypeID) const
	{
		auto it = mDataTypes.find(aDataTypeID);
		if (it != mDataTypes.end())
		{
			return &it->second;
		}
		return nullptr;
	}
}