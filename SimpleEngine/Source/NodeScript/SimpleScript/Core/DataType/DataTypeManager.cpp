#include "DataTypeManager.h"

namespace SCR
{

	bool DataTypeManager::EditData(DataTypeID aDataTypeID, void* aDataPtr)
	{
		auto it = myDataTypes.find(aDataTypeID);
		if (it != myDataTypes.end())
		{
			const DataType& dataType = it->second;
			if (dataType.typeInterface.function.edit)
			{
				return dataType.typeInterface.function.edit(aDataPtr);
			}
		}
		return false;
	}

	void DataTypeManager::SaveData(DataTypeID aDataTypeID, nlohmann::json& aJson, const void* aDataPtr)
	{
		auto it = myDataTypes.find(aDataTypeID);
		if (it != myDataTypes.end())
		{
			const DataType& dataType = it->second;
			if (dataType.typeInterface.function.save)
			{
				dataType.typeInterface.function.save(aJson, aDataPtr);
			}
		}
	}

	void DataTypeManager::LoadData(DataTypeID aDataTypeID, const nlohmann::json& aJson, void* aDataPtr)
	{
		auto it = myDataTypes.find(aDataTypeID);
		if (it != myDataTypes.end())
		{
			const DataType& dataType = it->second;
			if (dataType.typeInterface.function.load)
			{
				dataType.typeInterface.function.load(aJson, aDataPtr);
			}
		}
	}

	MemoryPoolID DataTypeManager::AllocateData(DataTypeID aDataTypeID, MemoryPool& aMemoryPool)
	{
		auto it = myDataTypes.find(aDataTypeID);
		if (it != myDataTypes.end())
		{
			const DataType& dataType = it->second;
			if (dataType.typeInterface.creation.allocate)
			{
				return dataType.typeInterface.creation.allocate(aMemoryPool);
			}
		}
		return false;
	}

	void DataTypeManager::CopyData(DataTypeID aDataTypeID, void* aDestination, const void* aSource)
	{
		auto it = myDataTypes.find(aDataTypeID);
		if (it != myDataTypes.end())
		{
			const DataType& dataType = it->second;
			if (dataType.typeInterface.creation.copy)
			{
				dataType.typeInterface.creation.copy(aDestination, aSource);
			}
		}
	}

	void DataTypeManager::SwapData(DataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2)
	{
		auto it = myDataTypes.find(aDataTypeID);
		if (it != myDataTypes.end())
		{
			const DataType& dataType = it->second;
			if (dataType.typeInterface.creation.swap)
			{
				dataType.typeInterface.creation.swap(aDataPtr1, aDataPtr2);
			}
		}
	}

	const std::string& DataTypeManager::GetName(DataTypeID aDataTypeID)
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

	const std::unordered_map<DataTypeID, DataType>& DataTypeManager::GetObjectTypes()
	{
		return myDataTypes;
	}

	std::unordered_map<DataTypeID, const DataType*> DataTypeManager::GetFunctionObjectTypes()
	{
		std::unordered_map<DataTypeID, const DataType*> funcObjectTypes;

		for (const auto& [dataTypeID, dataType] : myDataTypes)
		{
			if (dataType.typeInterface)
			{
				funcObjectTypes.emplace(dataTypeID, &dataType);
			}
		}

		return funcObjectTypes;
	}

	ScriptColor DataTypeManager::GetColor(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			const DataType& dataType = myDataTypes.at(aDataTypeID);
			return dataType.color;
		}
		return DefaultColor;
	}

	ScriptColor DataTypeManager::GetSelectionColor(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			const DataType& dataType = myDataTypes.at(aDataTypeID);
			return dataType.color - mySelectionTint;
		}
		return DefaultColor - mySelectionTint;
	}

	ScriptColor DataTypeManager::GetHoverColor(const DataTypeID aDataTypeID)
	{
		if (myDataTypes.contains(aDataTypeID))
		{
			const DataType& dataType = myDataTypes.at(aDataTypeID);
			return dataType.color - myHoverTint;
		}
		return DefaultColor - myHoverTint;
	}

	void DataTypeManager::Destroy()
	{
		myDataTypes.clear();
		myTemplateDataTypes.clear();
		myNullNameStr.~basic_string();
	}
}