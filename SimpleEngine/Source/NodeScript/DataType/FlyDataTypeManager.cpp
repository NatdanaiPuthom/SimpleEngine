#include "FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeManager::DataTypeManager()
	{
	}

	DataTypeManager::~DataTypeManager()
	{
	}

	bool DataTypeManager::EditData(const DataType& aDataType, void* const aDataPtr) const
	{
		// If data type has a valid edit function
		if (aDataType.mInterface.function.edit)
		{
			return aDataType.mInterface.function.edit(aDataPtr);
		}

		bool wasEdited = false;

		// Visualize properties instead
		for (const Variable& variable : aDataType.mVariables)
		{
			if (const DataType* variableDataType = Find(variable.mDataTypeID))
			{
				void* const propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + variable.mByteOffset);
				wasEdited |= EditData(*variableDataType, propertyDataPtr);
			}
		}

		return wasEdited;
	}

	bool DataTypeManager::SaveData(const DataType& aDataType, nlohmann::json& aJson, const void* aDataPtr) const
	{
		// If data type has a valid save function
		if (aDataType.mInterface.function.save)
		{
			aDataType.mInterface.function.save(aJson, aDataPtr);
			return true;
		}

		// Save properties instead
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

		// Load properties instead
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

	bool DataTypeManager::EditData(const DataTypeID aDataTypeID, void* const aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return EditData(*dataType, aDataPtr);
		}

		return false;
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
			if (dataType->mInterface.creation.release)
			{
				dataType->mInterface.creation.release(aDataPtr);
			}
		}
	}

	void DataTypeManager::CopyData(const DataTypeID aDataTypeID, void* const aDestination, const void* const aSource) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.creation.copy)
			{
				dataType->mInterface.creation.copy(aDestination, aSource);
			}
		}
	}

	void DataTypeManager::SwapData(const DataTypeID aDataTypeID, void* const aDataPtr1, void* const aDataPtr2) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.creation.swap)
			{
				dataType->mInterface.creation.swap(aDataPtr1, aDataPtr2);
			}
		}
	}

	const std::string& DataTypeManager::GetName(const DataTypeID aDataTypeID) const
	{
		if (mDataTypes.contains(aDataTypeID))
		{
			return mDataTypes.at(aDataTypeID).mName;
		}
		return mNullNameStr;
	}

	SetPinDataInterface DataTypeManager::GetSetPinDataInterface(DataTypeID aDataTypeID, eFlowType aFlowType) const
	{

		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SelectByFlowType(aFlowType, dataType->mInterface.execution.setInputPinData, dataType->mInterface.execution.setOutputPinData);
		}

		assert(false);
		return SetPinDataInterface();
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