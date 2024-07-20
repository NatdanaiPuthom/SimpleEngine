#include "DataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeManager::DataTypeManager()
	{
	}

	DataTypeManager::~DataTypeManager()
	{
	}

	bool DataTypeManager::EditData(const DataTypeID aDataTypeID, void* const aDataPtr)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			// If data type has a valid edit function
			if (dataType->mInterface.function.edit)
			{
				return dataType->mInterface.function.edit(aDataPtr);
			}

			bool wasEdited = false;

			// Visualize properties instead
			for (const Property& property : dataType->mProperties)
			{
				if (const DataType* propertyDataType = Find(property.mTypeID))
				{
					void* propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + property.byteOffset);
					wasEdited |= EditData(property.mTypeID, propertyDataPtr);
				}
			}

			return wasEdited;
		}

		return false;
	}

	bool DataTypeManager::SaveData(const DataTypeID aDataTypeID, nlohmann::json& aJson, const void* const aDataPtr)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			// If data type has a valid save function
			if (dataType->mInterface.function.save)
			{
				dataType->mInterface.function.save(aJson, aDataPtr);
				return true;
			}

			// Save properties instead
			for (const Property& property : dataType->mProperties)
			{
				nlohmann::json propertyJson;
				const void* propertyDataPtr = reinterpret_cast<const void*>(reinterpret_cast<size_t>(aDataPtr) + property.byteOffset);
				if (SaveData(property.mTypeID, propertyJson, propertyDataPtr))
				{
					aJson[property.mName] = propertyJson;
				}
			}
		}
		return false;
	}

	bool DataTypeManager::LoadData(const DataTypeID aDataTypeID, const nlohmann::json& aJson, void* const aDataPtr)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			// If data type has a valid load function
			if (dataType->mInterface.function.load)
			{
				dataType->mInterface.function.load(aJson, aDataPtr);
				return true;
			}

			// Load properties instead
			for (const Property& property : dataType->mProperties)
			{
				auto it = aJson.find(property.mName);
				if (it != aJson.end())
				{
					void* propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + property.byteOffset);
					LoadData(property.mTypeID, it.value(), propertyDataPtr);
				}
			}
		}
		return false;
	}

	void DataTypeManager::ReleaseData(const DataTypeID aDataTypeID, void* const aDataPtr)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.creation.release)
			{
				dataType->mInterface.creation.release(aDataPtr);
			}
		}
	}

	void DataTypeManager::CopyData(const DataTypeID aDataTypeID, void* const aDestination, const void* const aSource)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.creation.copy)
			{
				dataType->mInterface.creation.copy(aDestination, aSource);
			}
		}
	}

	void DataTypeManager::SwapData(const DataTypeID aDataTypeID, void* const aDataPtr1, void* const aDataPtr2)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.creation.swap)
			{
				dataType->mInterface.creation.swap(aDataPtr1, aDataPtr2);
			}
		}
	}

	const std::string& DataTypeManager::GetName(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			return myDataTypes.at(aDataTypeID).mName;
		}
		return myNullNameStr;
	}

	DataTypeID DataTypeManager::GetDataTypeIDByName(const std::string& aName)
	{
		for (const auto& [dataTypeID, dataType] : myDataTypes)
		{
			if (aName == dataType.mName)
			{
				return dataTypeID;
			}
		}
		return InvalidID<DataTypeID>();
	}

	const std::unordered_map<DataTypeID, DataType>& DataTypeManager::GetDataTypes()
	{
		return myDataTypes;
	}

	std::unordered_map<DataTypeID, const DataType*> DataTypeManager::GetFunctionDataTypes()
	{
		std::unordered_map<DataTypeID, const DataType*> dataTypes;

		for (const auto& [dataTypeID, dataType] : myDataTypes)
		{
			if (dataType.mInterface)
			{
				dataTypes.emplace(dataTypeID, &dataType);
			}
		}

		return dataTypes;
	}

	std::unordered_map<DataTypeID, const DataType*> DataTypeManager::GetDataTypesFiltered(const eDataTypeTrait aTrait, const eBitwiseType aBitwiseType)
	{
		std::unordered_map<DataTypeID, const DataType*> dataTypes;

		bool(*const predicate)(eDataTypeTrait, eDataTypeTrait) = aBitwiseType == eBitwiseType::HasFlag ? HasFlag<eDataTypeTrait> : Equals<eDataTypeTrait>;

		for (const auto& [dataTypeID, dataType] : myDataTypes)
		{
			if (predicate(dataType.mTypeTraits, aTrait))
			{
				dataTypes.emplace(dataTypeID, &dataType);
			}
		}

		return dataTypes;
	}

	DataType* DataTypeManager::Find(const DataTypeID anID)
	{
		auto it = myDataTypes.find(anID);

		if (it != myDataTypes.end())
		{
			return &it->second;
		}

		return nullptr;
	}
}