#include "DataTypeManager.h"

namespace SCR
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
			if (dataType->typeInterface.function.edit)
			{
				return dataType->typeInterface.function.edit(aDataPtr);
			}

			bool wasEdited = false;

			// Visualize properties instead
			for (const Property& property : dataType->properties)
			{
				if (const DataType* propertyDataType = Find(property.typeID))
				{
					void* propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + property.byteOffset);
					wasEdited |= EditData(property.typeID, propertyDataPtr);
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
			if (dataType->typeInterface.function.save)
			{
				dataType->typeInterface.function.save(aJson, aDataPtr);
				return true;
			}

			// Save properties instead
			for (const Property& property : dataType->properties)
			{
				nlohmann::json propertyJson;
				const void* propertyDataPtr = reinterpret_cast<const void*>(reinterpret_cast<size_t>(aDataPtr) + property.byteOffset);
				if (SaveData(property.typeID, propertyJson, propertyDataPtr))
				{
					aJson[property.name] = propertyJson;
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
			if (dataType->typeInterface.function.load)
			{
				dataType->typeInterface.function.load(aJson, aDataPtr);
				return true;
			}

			// Load properties instead
			for (const Property& property : dataType->properties)
			{
				auto it = aJson.find(property.name);
				if (it != aJson.end())
				{
					void* propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + property.byteOffset);
					LoadData(property.typeID, it.value(), propertyDataPtr);
				}
			}
		}
		return false;
	}

	void DataTypeManager::ReleaseData(const DataTypeID aDataTypeID, void* const aDataPtr)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->typeInterface.creation.release)
			{
				dataType->typeInterface.creation.release(aDataPtr);
			}
		}
	}

	void DataTypeManager::CopyData(const DataTypeID aDataTypeID, void* const aDestination, const void* const aSource)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->typeInterface.creation.copy)
			{
				dataType->typeInterface.creation.copy(aDestination, aSource);
			}
		}
	}

	void DataTypeManager::SwapData(const DataTypeID aDataTypeID, void* const aDataPtr1, void* const aDataPtr2)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->typeInterface.creation.swap)
			{
				dataType->typeInterface.creation.swap(aDataPtr1, aDataPtr2);
			}
		}
	}

	const std::string& DataTypeManager::GetName(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			return myDataTypes.at(aDataTypeID).name;
		}
		return myNullNameStr;
	}

	DataTypeID DataTypeManager::GetDataTypeIDByName(const std::string& aName)
	{
		for (const auto& [dataTypeID, dataType] : myDataTypes)
		{
			if (aName == dataType.name)
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
			if (dataType.typeInterface)
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
			if (predicate(dataType.typeTraits, aTrait))
			{
				dataTypes.emplace(dataTypeID, &dataType);
			}
		}

		return dataTypes;
	}

	Color DataTypeManager::GetColor(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			const DataType& dataType = myDataTypes.at(aDataTypeID);
			return dataType.color;
		}
		return DefaultColor;
	}

	Color DataTypeManager::GetSelectionColor(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			const DataType& dataType = myDataTypes.at(aDataTypeID);
			return dataType.color - mySelectionTint;
		}
		return DefaultColor - mySelectionTint;
	}

	Color DataTypeManager::GetHoverColor(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			const DataType& dataType = myDataTypes.at(aDataTypeID);
			return dataType.color - myHoverTint;
		}
		return DefaultColor - myHoverTint;
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