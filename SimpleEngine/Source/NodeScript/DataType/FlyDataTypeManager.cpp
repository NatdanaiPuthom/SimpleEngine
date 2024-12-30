#include "FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeManager::DataTypeManager()
	{
	}

	DataTypeManager::~DataTypeManager()
	{
	}

	ViewAndEditResult DataTypeManager::ViewAndEditData(const DataType& aDataType, void* const aDataPtr) const
	{
		// If data type has a valid edit function
		if (aDataType.mInterface.function.viewAndEdit)
		{
			return aDataType.mInterface.function.viewAndEdit(aDataPtr);
		}
		else if (HasFlag(aDataType.mTypeTraits, eDataTypeTrait::Pointer))
		{
			if (const DataType* valueDataType = Find(aDataType.mToValueDataTypeID))
			{
				void* valuePtr = *((void**)aDataPtr);
				if (valuePtr)
				{
					return ViewAndEditData(*valueDataType, valuePtr);
				}
			}
		}

		ViewAndEditResult viewAndEditResult;
		// View and edit member variables instead
		/*for (const Variable& variable : aDataType.mVariables)
		{
			if (const DataType* variableDataType = Find(variable.mDataTypeID))
			{
				void* const propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + variable.mByteOffset);
				viewAndEditResult.mIsItemActive |= ViewAndEditData(*variableDataType, propertyDataPtr).mIsItemActive;
			}
		}*/

		return viewAndEditResult;
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

	bool DataTypeManager::SaveData(const DataType& aDataType, const void* aDataPtr, nlohmann::json& aJson) const
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
				if (SaveData(*variableDataType, variableDataPtr, variableJson))
				{
					aJson[variable.mName] = variableJson;
				}
			}
		}

		return false;
	}

	bool DataTypeManager::LoadData(const DataType& aDataType, void* const aDataPtr, const nlohmann::json& aJson) const
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
					LoadData(*dataType, propertyDataPtr, it.value());
				}
			}
		}

		return false;
	}

	ViewAndEditResult DataTypeManager::ViewAndEditData(const DataTypeID aDataTypeID, void* const aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return ViewAndEditData(*dataType, aDataPtr);
		}

		return ViewAndEditResult{ .mIsItemActive = false };
	}

	void DataTypeManager::ViewData(DataTypeID aDataTypeID, const void* aDataPtr) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			ViewData(*dataType, aDataPtr);
		}
	}

	bool DataTypeManager::SaveData(const DataTypeID aDataTypeID, const void* const aDataPtr, nlohmann::json& aJson) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SaveData(*dataType, aDataPtr, aJson);
		}
		return false;
	}

	bool DataTypeManager::LoadData(const DataTypeID aDataTypeID, void* const aDataPtr, const nlohmann::json& aJson) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return LoadData(*dataType, aDataPtr, aJson);
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

	ViewAndEditResult DataTypeManager::ViewAndEditData(GenericDataTypeID aDataTypeID, void* aDataPtr) const
	{
		return std::visit(Visitor{
			[this, aDataPtr](const DataTypeID aDataTypeID) -> ViewAndEditResult
			{
				return ViewAndEditData(aDataTypeID, aDataPtr);
			},
			[this, aDataPtr](const StructID) -> ViewAndEditResult
			{
				return ViewAndEditData(GetDataTypeID<StructInstance>(), aDataPtr);
			},
			[this, aDataPtr](const ClassID) -> ViewAndEditResult
			{
				assert(false);
				return ViewAndEditData(GetDataTypeID<ClassInstance>(), aDataPtr);
			}
			}, aDataTypeID.mID);
	}

	void DataTypeManager::ViewData(GenericDataTypeID aDataTypeID, const void* aDataPtr) const
	{
		std::visit(Visitor{
			[this, aDataPtr](const DataTypeID aDataTypeID) -> void
			{
				ViewData(aDataTypeID, aDataPtr);
			},
			[this, aDataPtr](const StructID) -> void
			{
				ViewData(GetDataTypeID<StructInstance>(), aDataPtr);
			},
			[this, aDataPtr](const ClassID) -> void
			{
				assert(false);
				ViewData(GetDataTypeID<ClassInstance>(), aDataPtr);
			}
			}, aDataTypeID.mID);
	}

	bool DataTypeManager::SaveData(const GenericDataTypeID aDataTypeID, const void* const  aDataPtr, nlohmann::json& aJson) const
	{
		return std::visit(Visitor{
			[this, aDataPtr, &aJson](const DataTypeID aDataTypeID) -> bool
			{
				return SaveData(aDataTypeID, aDataPtr, aJson);
			},
			[this, aDataPtr, &aJson](const StructID) -> bool
			{
				return SaveData(GetDataTypeID<StructInstance>(), aDataPtr, aJson);
			},
			[this, aDataPtr, &aJson](const ClassID) -> bool
			{
				assert(false);
				return SaveData(GetDataTypeID<ClassInstance>(), aDataPtr, aJson);
			}
			}, aDataTypeID.mID);
	}

	bool DataTypeManager::LoadData(const GenericDataTypeID aDataTypeID, void* const aDataPtr, const nlohmann::json& aJson) const
	{
		return std::visit(Visitor{
			[this, aDataPtr, &aJson](const DataTypeID aDataTypeID) -> bool
			{
				return LoadData(aDataTypeID, aDataPtr, aJson);
			},
			[this, aDataPtr, &aJson](const StructID) -> bool
			{
				return LoadData(GetDataTypeID<StructInstance>(), aDataPtr, aJson);
			},
			[this, aDataPtr, &aJson](const ClassID) -> bool
			{
				assert(false);
				return LoadData(GetDataTypeID<ClassInstance>(), aDataPtr, aJson);
			}
			}, aDataTypeID.mID);
	}

	void DataTypeManager::ReleaseData(const GenericDataTypeID aDataTypeID, void* const aDataPtr) const
	{
		std::visit(Visitor{
			   [this, aDataPtr](const DataTypeID aDataTypeID) -> void
			   {
				   ReleaseData(aDataTypeID, aDataPtr);
			   },
			   [this, aDataPtr](const StructID) -> void
			   {
				   ReleaseData(GetDataTypeID<StructInstance>(), aDataPtr);
			   },
			   [this, aDataPtr](const ClassID) -> void
			   {
					assert(false);
			   }
			}, aDataTypeID.mID);
	}

	void DataTypeManager::CopyData(GenericDataTypeID aDataTypeID, void* aDestination, const void* aSource) const
	{
		std::visit(Visitor{
			[this, aDestination, aSource](const DataTypeID aDataTypeID) -> void
			{
				CopyData(aDataTypeID, aDestination, aSource);
			},
			[this, aDestination, aSource](const StructID) -> void
			{
				CopyData(GetDataTypeID<StructInstance>(), aDestination, aSource);
			},
			[this, aDestination, aSource](const ClassID) -> void
			{
				assert(false);
				CopyData(GetDataTypeID<ClassInstance>(), aDestination, aSource);
			}
			}, aDataTypeID.mID);
	}

	void DataTypeManager::SwapData(GenericDataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2) const
	{
		std::visit(Visitor{
			[this, aDataPtr1, aDataPtr2](const DataTypeID aDataTypeID) -> void
			{
				SwapData(aDataTypeID, aDataPtr1, aDataPtr2);
			},
			[this, aDataPtr1, aDataPtr2](const StructID) -> void
			{
				SwapData(GetDataTypeID<StructInstance>(), aDataPtr1, aDataPtr2);
			},
			[this, aDataPtr1, aDataPtr2](const ClassID) -> void
			{
				assert(false);
				SwapData(GetDataTypeID<ClassInstance>(), aDataPtr1, aDataPtr2);
			}
			}, aDataTypeID.mID);
	}

	bool DataTypeManager::AreDataTypesRelated(const DataTypeID aDataTypeID1, const DataTypeID aDataTypeID2) const
	{
		if (aDataTypeID1 == aDataTypeID2)
		{
			return true;
		}
		auto checker = [this](const DataTypeID a, const DataTypeID b) -> bool
			{
				if (const DataType* dataType = Find(a))
				{
					return dataType->mToPointerDataTypeID == b || dataType->mToValueDataTypeID == b;
				}
				return false;
			};

		return checker(aDataTypeID1, aDataTypeID2) || checker(aDataTypeID2, aDataTypeID1);
	}

	bool DataTypeManager::AreDataTypesRelated(GenericDataTypeID aDataTypeID1, GenericDataTypeID aDataTypeID2) const
	{
		return std::visit(Visitor{
			[this](const DataTypeID aDataTypeID1, const DataTypeID aDataTypeID2)
			{
				return AreDataTypesRelated(aDataTypeID1, aDataTypeID2);
			},
			[](const StructID aStructID1, const StructID aStructID2)
			{
				return aStructID1 == aStructID2;
			},
			[](const ClassID aClassID1, const ClassID aClassID2)
			{
				return aClassID1 == aClassID2;
			},
			[](const auto, const auto)
			{
				return false;
			}
			}, aDataTypeID1.mID, aDataTypeID2.mID);
	}

	eDataTypeRelation DataTypeManager::GetDataTypeRelation(const DataTypeID aDataTypeID1, const DataTypeID aDataTypeID2) const
	{
		if (aDataTypeID1 == aDataTypeID2)
		{
			return eDataTypeRelation::Same;
		}

		if (const DataType* dataType = Find(aDataTypeID1))
		{
			if (dataType->mToPointerDataTypeID == aDataTypeID2)
			{
				return eDataTypeRelation::Value_Pointer;
			}
			else if (dataType->mToValueDataTypeID == aDataTypeID2)
			{
				return eDataTypeRelation::Pointer_Value;
			}
		}

		return eDataTypeRelation::None;

	}

	eDataTypeRelation DataTypeManager::GetDataTypeRelation(const GenericDataTypeID aDataTypeID1, const GenericDataTypeID aDataTypeID2) const
	{
		return std::visit(Visitor{
			[this](const DataTypeID aDataTypeID1, const DataTypeID aDataTypeID2)
			{
				return GetDataTypeRelation(aDataTypeID1, aDataTypeID2);
			},
			[](const StructID aStructID1, const StructID aStructID2)
			{
				if (aStructID1 == aStructID2)
				{
					return eDataTypeRelation::Same;
				}
				return eDataTypeRelation::None;
			},
			[](const ClassID aClassID1, const ClassID aClassID2)
			{
				if (aClassID1 == aClassID2)
				{
					return eDataTypeRelation::Same;
				}
				return eDataTypeRelation::None;
			},
			[](const auto, const auto)
			{
				return eDataTypeRelation::None;
			}
			}, aDataTypeID1.mID, aDataTypeID2.mID);
	}

	const std::string& DataTypeManager::GetName(const DataTypeID aDataTypeID) const
	{
		if (mDataTypes.contains(aDataTypeID))
		{
			return mDataTypes.at(aDataTypeID).mName;
		}
		return mNullNameStr;
	}

	std::string_view DataTypeManager::GetName(GenericDataTypeID aDataTypeID) const
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> std::string_view
			{
				return GetName(aDataTypeID);
			},
			[this](const StructID aStructID) -> std::string_view
			{
				return GetStruct(aStructID).mName;
			},
			[this](const ClassID aClassID) -> std::string_view
			{
				return GetClass(aClassID).mName;
			}
			}, aDataTypeID.mID);
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

	void DataTypeManager::SetEditorNullptrFunction(void(*aFunction)())
	{
		editorNullptrFunction = aFunction;
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

	const DataType* DataTypeManager::Find(const DataTypeID aDataTypeID) const
	{
		auto it = mDataTypes.find(aDataTypeID);
		if (it != mDataTypes.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	GenericDataTypePtr DataTypeManager::Find(GenericDataTypeID aDataTypeID)
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> GenericDataTypePtr
			{
				return Find(aDataTypeID);
			},
			[this](const StructID aStructID) -> GenericDataTypePtr
			{
				return &GetStruct(aStructID);
			},
			[this](const ClassID aClassID) -> GenericDataTypePtr
			{
				return &GetClass(aClassID);
			}
			}, aDataTypeID.mID);
	}

	bool DataTypeManager::IsRegistered(const DataTypeID aDataTypeID) const
	{
		return mDataTypes.contains(aDataTypeID);
	}

	void DataTypeManager::SetDefaultColor(const Color& aColor)
	{
		mDefaultColor = aColor;
	}

	Color DataTypeManager::GetDefaultColor() const
	{
		return mDefaultColor;
	}

	StructID DataTypeManager::CreateStruct(const std::string_view aName)
	{
		StructID structID{ mStructs.size() };
		mStructs.push_back(HeapObject<Struct>(aName));
		return structID;
	}

	Struct& DataTypeManager::GetStruct(const StructID aStructID)
	{
		return *mStructs[aStructID];
	}

	const Struct& DataTypeManager::GetStruct(const StructID aStructID) const
	{
		return *mStructs[aStructID];
	}

	StructID DataTypeManager::GetStructIDByName(std::string_view aName) const
	{
		for (size_t i = 0; i < mStructs.size(); i++)
		{
			if (mStructs[i]->mName == aName)
			{
				return StructID{ i };
			}
		}

		return StructID{};
	}

	const std::vector<HeapObject<Struct>>& DataTypeManager::GetStructs() const
	{
		return mStructs;
	}

	ClassID DataTypeManager::CreateClass(const DataTypeID aTargetID, const std::string_view aName)
	{
		ClassID id{ mClasses.size() };
		mClasses.emplace_back(HeapObject<Class>(aTargetID, std::string(aName)));
		return id;
	}

	Class& DataTypeManager::GetClass(const ClassID aClassID)
	{
		return *mClasses[aClassID];
	}

	const Class& DataTypeManager::GetClass(const ClassID aClassID) const
	{
		return *mClasses[aClassID];
	}

	ClassID DataTypeManager::GetClassIDByName(std::string_view aName) const
	{
		for (size_t i = 0; i < mClasses.size(); i++)
		{
			if (mClasses[i]->mName == aName)
			{
				return ClassID{ i };
			}
		}

		return ClassID{};
	}


	const std::vector<HeapObject<Class>>& DataTypeManager::GetClasses() const
	{
		return mClasses;
	}
}