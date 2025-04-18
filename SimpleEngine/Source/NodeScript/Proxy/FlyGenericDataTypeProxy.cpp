#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "FlyGenericDataTypeProxy.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "FlyDataTypeProxy.hpp"

namespace FLY_NAMESPACE
{
	GenericDataTypeProxy::GenericDataTypeProxy(const GenericDataTypeID aDataTypeID)
		: mDataTypeID(aDataTypeID)
	{
	}

	GenericDataTypeProxy::GenericDataTypeProxy(const DataTypeProxy& aDataType)
		: mDataTypeID(GenericDataTypeID{ aDataType.GetID() })
	{
	}

	GenericDataTypeProxy::GenericDataTypeProxy(const DataTypeID aDataTypeID)
		: GenericDataTypeProxy(GenericDataTypeID{ aDataTypeID })
	{
	}

	GenericDataTypeProxy::GenericDataTypeProxy(const ClassID aClassID)
		: GenericDataTypeProxy(GenericDataTypeID{ aClassID })
	{
	}

	GenericDataTypeID GenericDataTypeProxy::GetID() const
	{
		return mDataTypeID;
	}

	const std::string& GenericDataTypeProxy::GetName() const
	{
		return Internal::GetDataTypeManager().GetName(mDataTypeID);
	}

	Color GenericDataTypeProxy::GetColor() const
	{
		return Internal::GetDataTypeManager().GetDataTypeColor(mDataTypeID);
	}

	eDataTypeTrait GenericDataTypeProxy::GetTypeTraits() const
	{
		return Internal::GetDataTypeManager().GetDataTypeTraits(mDataTypeID);
	}

	size_t GenericDataTypeProxy::GetSize() const
	{
		const DataType* dataType = GetDataType();
		if (dataType)
		{
			return dataType->GetSize();
		}
		return 0;
	}

	size_t GenericDataTypeProxy::GetAlignment() const
	{
		return Internal::GetDataTypeManager().GetDataTypeAlignment(mDataTypeID);
	}

	bool GenericDataTypeProxy::IsTargetable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Targetable);
	}

	bool GenericDataTypeProxy::IsPointer() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Pointer);
	}

	bool GenericDataTypeProxy::IsViewAndEditable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::ViewAndEditable);
	}

	bool GenericDataTypeProxy::IsViewable() const
	{
		return HasFlag(GetTypeTraits(), eDataTypeTrait::Viewable);
	}

	std::vector<VariableProxy> GenericDataTypeProxy::GetMemberVariables() const
	{
		const DataType* dataType = GetDataType();
		if (!dataType)
		{
			return {};
		}

		std::vector<VariableProxy> memberVariables;
		memberVariables.reserve(dataType->GetVariableContainer().GetVariableCount());
		for (VarID varID{ 0 }; varID < dataType->GetVariableContainer().GetVariableCount(); ++varID)
		{
			VariableProxy facade(varID, *this);
			memberVariables.push_back(facade);
		}
		return memberVariables;
	}

	void GenericDataTypeProxy::SetColor(const Color& aColor)
	{
		Internal::GetDataTypeManager().SetDataTypeColor(mDataTypeID, aColor);
	}

	VariableProxy GenericDataTypeProxy::CreateMemberVariable(GenericDataTypeProxy aDataTypeProxy, std::string_view aName, CommandTracker* const aCommandTracker)
	{
		/*VariableContainer& varContainer = std::visit(Visitor{
			[](const DataTypeID aDataTypeID) -> VariableContainer&
			{
				return Internal::GetDataTypeManager().Find(aDataTypeID)->mVariableContainer;
			},
			[](const ClassID aClassID) -> VariableContainer&
			{
				return Internal::GetDataTypeManager().Find(aClassID)->mVariableContainer;
			}
			}, mDataTypeID.mID);*/
		VarID varID = Internal::CreateVariable(GetID(), aDataTypeProxy.GetID(), aName, aCommandTracker);
		return VariableProxy(varID, *this);
	}

	GenericDataTypeProxy::operator bool() const
	{
		return mDataTypeID != InvalidID<GenericDataTypeID>();
	}

	bool GenericDataTypeProxy::operator==(const GenericDataTypeProxy& aOther) const
	{
		return mDataTypeID == aOther.mDataTypeID;
	}

	DataType* GenericDataTypeProxy::GetDataType() const
	{
		return Internal::GetDataTypeManager().Find(mDataTypeID);
	}
}