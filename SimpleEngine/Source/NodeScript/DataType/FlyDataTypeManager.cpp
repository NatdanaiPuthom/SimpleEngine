#include "FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	DataTypeManager::DataTypeManager()
	{
	}

	DataTypeManager::~DataTypeManager()
	{
	}

	ViewAndEditResult DataTypeManager::ViewAndEditData(const DataType& aDataType, void* const aDataPtr, const bool aViewAndEditMembers) const
	{
		// If data type has a valid edit function
		if (aDataType.GetInterface().GetViewAndEditFunction())
		{
			return aDataType.GetInterface().GetViewAndEditFunction().Invoke(aDataPtr, mEditorTextFunction);
		}
		else if (HasFlag(aDataType.GetTypeTraits(), eDataTypeTrait::Pointer))
		{
			if (const DataType* valueDataType = Find(aDataType.GetToValueDataTypeID()))
			{
				void* valuePtr = *((void**)aDataPtr);
				if (valuePtr)
				{
					return ViewAndEditData(*valueDataType, valuePtr, aViewAndEditMembers);
				}
			}
		}

		ViewAndEditResult viewAndEditResult;
		// View and edit member variables instead
		if (aViewAndEditMembers)
		{

			for (const Variable& variable : aDataType.GetVariableContainer())
			{
				if (const DataType* variableDataType = Find(variable.GetDataTypeID()))
				{
					void* const propertyDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + variable.GetByteOffset());
					viewAndEditResult.mIsItemActive |= ViewAndEditData(*variableDataType, propertyDataPtr, aViewAndEditMembers).mIsItemActive;
				}
			}
		}

		return viewAndEditResult;
	}

	void DataTypeManager::ViewData(const DataType& aDataType, const void* const aDataPtr) const
	{
		// If data type has a valid edit function
		if (aDataType.GetInterface().GetViewFunction())
		{
			aDataType.GetInterface().GetViewFunction().Invoke(aDataPtr, mEditorTextFunction);
			return;
		}

		// View member variables instead
		for (const Variable& variable : aDataType.GetVariableContainer())
		{
			if (const DataType* variableDataType = Find(variable.GetDataTypeID()))
			{
				const void* const dataPtr = reinterpret_cast<const void*>(reinterpret_cast<size_t>(aDataPtr) + variable.GetByteOffset());
				ViewData(*variableDataType, dataPtr);
			}
		}

	}

	bool DataTypeManager::SaveData(const DataType& aDataType, const void* aDataPtr, nlohmann::json& aJson) const
	{
		// If data type has a valid save function
		if (aDataType.GetInterface().GetSaveFunction())
		{
			aDataType.GetInterface().GetSaveFunction().Invoke(aDataPtr, aJson);
			return true;
		}

		// Save member variables instead
		for (const Variable& variable : aDataType.GetVariableContainer())
		{
			if (const DataType* variableDataType = Find(variable.GetDataTypeID()))
			{
				nlohmann::json variableJson;
				const void* variableDataPtr = reinterpret_cast<const void*>(reinterpret_cast<size_t>(aDataPtr) + variable.GetByteOffset());
				if (SaveData(*variableDataType, variableDataPtr, variableJson))
				{
					aJson[variable.Name()] = variableJson;
				}
			}
		}

		return false;
	}

	bool DataTypeManager::LoadData(const DataType& aDataType, void* const aDataPtr, const nlohmann::json& aJson) const
	{
		// If data type has a valid load function
		if (aDataType.GetInterface().GetLoadFunction())
		{
			aDataType.GetInterface().GetLoadFunction().Invoke(aDataPtr, aJson);
			return true;
		}

		// Load member variables instead
		for (const Variable& variable : aDataType.GetVariableContainer())
		{
			if (const DataType* dataType = Find(variable.GetDataTypeID()))
			{
				auto it = aJson.find(variable.Name());
				if (it == aJson.end())
				{
					continue;
				}

				void* const variableDataPtr = reinterpret_cast<void*>(reinterpret_cast<size_t>(aDataPtr) + variable.GetByteOffset());
				LoadData(*dataType, variableDataPtr, it.value());
			}
		}

		return false;
	}

	ViewAndEditResult DataTypeManager::ViewAndEditData(const DataTypeID aDataTypeID, void* const aDataPtr, const bool aViewAndEditMembers) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return ViewAndEditData(*dataType, aDataPtr, aViewAndEditMembers);
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
			if (dataType->GetInterface().GetReleaseFunction())
			{
				dataType->GetInterface().GetReleaseFunction().Invoke(aDataPtr);
			}
		}
	}

	void DataTypeManager::CopyData(const DataTypeID aDataTypeID, void* const aDestination, const void* const aSource) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->GetInterface().GetCopyFunction())
			{
				dataType->GetInterface().GetCopyFunction().Invoke(aDestination, aSource);
			}
		}
	}

	void DataTypeManager::SwapData(const DataTypeID aDataTypeID, void* const aDataPtr1, void* const aDataPtr2) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->GetInterface().GetSwapFunction())
			{
				dataType->GetInterface().GetSwapFunction().Invoke(aDataPtr1, aDataPtr2);
			}
		}
	}

	bool DataTypeManager::DataEqualsTo(const DataTypeID aDataTypeID, const void* const aDataPtr1, const void* const aDataPtr2) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->GetInterface().GetEqualsFunction())
			{
				return dataType->GetInterface().GetEqualsFunction().Invoke(aDataPtr1, aDataPtr2);
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

	bool DataTypeManager::DataEqualsTo(const GenericDataTypeID aDataTypeID, const void* const aDataPtr1, const void* const aDataPtr2) const
	{
		return std::visit(Visitor{
			[this, aDataPtr1, aDataPtr2](const DataTypeID aDataTypeID) -> bool
			{
				return DataEqualsTo(aDataTypeID, aDataPtr1, aDataPtr2);
			},
			[this, aDataPtr1, aDataPtr2](const StructID) -> bool
			{
				return DataEqualsTo(GetDataTypeID<StructInstance>(), aDataPtr1, aDataPtr2);
			},
			[this, aDataPtr1, aDataPtr2](const ClassID) -> bool
			{
				assert(false);
				return DataEqualsTo(GetDataTypeID<ClassInstance>(), aDataPtr1, aDataPtr2);
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
					return dataType->GetToPointerDataTypeID() == b || dataType->GetToValueDataTypeID() == b;
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
			if (dataType->GetToPointerDataTypeID() == aDataTypeID2)
			{
				return eDataTypeRelation::Value_Pointer;
			}
			else if (dataType->GetToValueDataTypeID() == aDataTypeID2)
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

	const std::string& DataTypeManager::GetName(GenericDataTypeID aDataTypeID) const
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> const std::string&
			{
				return Find(aDataTypeID)->Name();
			},
			[this](const ClassID aClassID) -> const std::string&
			{
				return GetClass(aClassID).mName;
			}
			}, aDataTypeID.mID);
	}

	Color DataTypeManager::GetDataTypeColor(GenericDataTypeID aDataTypeID) const
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> Color
			{
				if (const DataType* dataType = Find(aDataTypeID))
				{
					return dataType->GetColor();
				}
				return mDefaultColor;
			},
			[this](const StructID) -> Color
			{
				return mDefaultStructColor;
			},
			[this](const ClassID) -> Color
			{
				return mDefaultClassColor;
			}
			}, aDataTypeID.mID);
	}

	eDataTypeTrait DataTypeManager::GetDataTypeTraits(GenericDataTypeID aDataTypeID) const
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> eDataTypeTrait
			{
				if (const DataType* dataType = Find(aDataTypeID))
				{
					return dataType->GetTypeTraits();
				}
				return eDataTypeTrait::None;
			},
			[](const StructID) -> eDataTypeTrait
			{
				return eDataTypeTrait::None;
			},
			[](const ClassID) -> eDataTypeTrait
			{
				return eDataTypeTrait::None;
			}
			}, aDataTypeID.mID);
	}

	size_t DataTypeManager::GetDataTypeSize(GenericDataTypeID aDataTypeID) const
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> size_t
			{
				if (const DataType* dataType = Find(aDataTypeID))
				{
					return dataType->GetSize();
				}
				return 0;
			},
			[](const StructID) -> size_t
			{
				return 0;
			},
			[](const ClassID) -> size_t
			{
				return 0;
			}
			}, aDataTypeID.mID);
	}

	size_t DataTypeManager::GetDataTypeAlignment(GenericDataTypeID aDataTypeID) const
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> size_t
			{
				if (const DataType* dataType = Find(aDataTypeID))
				{
					return dataType->GetAlignment();
				}
				return 0;
			},
			[](const StructID) -> size_t
			{
				return 0;
			},
			[](const ClassID) -> size_t
			{
				return 0;
			}
			}, aDataTypeID.mID);
	}

	SetPinValueF DataTypeManager::GetSetPinValueFunction(const DataTypeID aDataTypeID, const eIODirection aIODirection) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SelectByIODirection(aIODirection, dataType->GetInterface().GetSetInputPinValueFunction(), dataType->GetInterface().GetSetOutputPinValueFunction());
		}

		assert(false);
		return SetPinValueF{};
	}

	SetPinValueF DataTypeManager::GetSetPinValueFunction(GenericDataTypeID aDataTypeID, eIODirection aIODirection) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SelectByIODirection(aIODirection, dataType->GetInterface().GetSetInputPinValueFunction(), dataType->GetInterface().GetSetOutputPinValueFunction());
		}

		assert(false);
		return SetPinValueF{};
	}

	SetPinValueFromPinF DataTypeManager::GetSetPinValueFromPinFunction(const DataTypeID aDataTypeID, const eIODirection aIODirection) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SelectByIODirection(aIODirection, dataType->GetInterface().GetSetInputPinValueFromPinFunction(), dataType->GetInterface().GetSetOutputPinValueFromPinFunction());
		}

		assert(false);
		return SetPinValueFromPinF{};
	}

	SetPinValueFromPinF DataTypeManager::GetSetPinValueFromPinFunction(GenericDataTypeID aDataTypeID, const eIODirection aIODirection) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			return SelectByIODirection(aIODirection, dataType->GetInterface().GetSetInputPinValueFromPinFunction(), dataType->GetInterface().GetSetOutputPinValueFromPinFunction());
		}

		assert(false);
		return SetPinValueFromPinF{};
	}

	DataTypeID DataTypeManager::GetDataTypeIDByName(std::string_view aName) const
	{
		for (const auto& [dataTypeID, dataType] : mDataTypes)
		{
			if (aName == dataType.Name())
			{
				return dataTypeID;
			}
		}
		return InvalidID<DataTypeID>();
	}

	GenericDataTypeID DataTypeManager::GetGenericDataTypeIDByName(const std::string_view aName) const
	{
		const DataTypeID dataTypeID = GetDataTypeIDByName(aName);
		if (dataTypeID != InvalidID<DataTypeID>())
		{
			return GenericDataTypeID{ dataTypeID };
		}

		for (size_t i = 0; i < mClasses.size(); i++)
		{
			if (std::string_view(mClasses[i]->mName) == aName)
			{
				return GenericDataTypeID{ ClassID{ i } };
			}
		}
		return GenericDataTypeID{};
	}

	EditorTextFunction DataTypeManager::GetEditorTextFunction() const
	{
		return mEditorTextFunction;
	}


	void DataTypeManager::SetEditorTextFunction(EditorTextFunction aTextFunction)
	{
		mEditorTextFunction = aTextFunction;
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

	DataType* DataTypeManager::Find(ClassID aClassID)
	{
		return mClassesNew[aClassID].Get();
	}

	const DataType* DataTypeManager::Find(ClassID aClassID) const
	{
		return mClassesNew[aClassID].Get();
	}

	DataType* DataTypeManager::Find(GenericDataTypeID aDataTypeID)
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> DataType*
			{
				return Find(aDataTypeID);
			},
			[this](const ClassID aClassID) -> DataType*
			{
				return Find(aClassID);
			}
			}, aDataTypeID.mID);
	}

	const DataType* DataTypeManager::Find(const GenericDataTypeID aDataTypeID) const
	{
		return std::visit(Visitor
			{
			[this](const DataTypeID aDataTypeID) -> const DataType*
			{
				return Find(aDataTypeID);
			},
			[this](const ClassID aClassID) -> const DataType*
			{
				return Find(aClassID);
			}
			}, aDataTypeID.mID);
	}

	bool DataTypeManager::IsRegistered(const DataTypeID aDataTypeID) const
	{
		return mDataTypes.contains(aDataTypeID);
	}

	void DataTypeManager::SetDataTypeColor(GenericDataTypeID aDataTypeID, const Color& aColor)
	{
		std::visit(Visitor{
			[&](const DataTypeID aDataTypeID) -> void
			{
				if (DataType* dataType = Find(aDataTypeID))
				{
					dataType->SetColor(aColor);
				}
			},
			[&](const auto) -> void
			{
				assert(false);
			},
			}, aDataTypeID.mID);
	}

	void DataTypeManager::SetDefaultColor(const Color& aColor)
	{
		mDefaultColor = aColor;
	}

	Color DataTypeManager::GetDefaultColor() const
	{
		return mDefaultColor;
	}

	DataTypeID DataTypeManager::CreateStruct(const std::string_view aName)
	{
		DataType dataType;

		const DataType* structInstanceDataType = Find<StructInstance>();

		if (!structInstanceDataType)
		{
			assert(false);
		}

		while (mDataTypes.contains(mDataTypeIDCounter))
		{
			mDataTypeIDCounter++;
		}

		auto pair = mDataTypes.emplace(mDataTypeIDCounter, *structInstanceDataType);
		pair.first->second.Name() = std::string(aName);
		const DataTypeID dataTypeID = mDataTypeIDCounter;
		mDataTypeIDCounter++;
		return dataTypeID;
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