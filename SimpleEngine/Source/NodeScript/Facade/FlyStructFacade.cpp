#include "FlyStructFacade.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../Internal/FlyInternal.hpp"
#include "FlyDataTypeFacade.hpp"

namespace FLY_NAMESPACE
{

	

	StructFacade::StructFacade(const StructID aStructID)
		: mStructID(aStructID)
	{
	}

	std::string_view StructFacade::GetName() const
	{
		return GetStruct().mName;
	}

	StructID StructFacade::GetID() const
	{
		return mStructID;
	}

	std::vector<VariableFacade> StructFacade::GetMemberVariables(const bool aIncludeDestroyed) const
	{
		std::vector<VariableFacade> variableFacades;

		Struct& s = GetStruct();

		variableFacades.reserve(s.mVariableContainer.mVariables.size());
		for (size_t i = 0; i < s.mVariableContainer.mVariables.size(); i++)
		{
			if (!aIncludeDestroyed && s.mVariableContainer.mVariables[i].mIsDestroyed)
			{
				continue;
			}
			variableFacades.push_back(VariableFacade(VarID{ i }, *this));
		}

		return variableFacades;

	}

	VariableFacade StructFacade::CreateVariable(const DataTypeFacade& aDataTypeFacade, const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		VarID varID = Internal::CreateVariable(GetStruct().mVariableContainer, aDataTypeFacade.GetID(), aName, aCommandTracker);
		return VariableFacade(varID, *this);
	}

	void StructFacade::SetName(std::string_view aName, CommandTracker* const aCommandTracker)
	{
		Internal::SetStructName(mStructID, aName, aCommandTracker);
	}

	Struct& StructFacade::GetStruct() const
	{
		return Internal::GetStructByID(mStructID);
	}

	StructFacade::operator bool() const
	{
		return mStructID != InvalidID<StructID>();
	}

	bool operator==(const StructFacade& a, const StructFacade& b)
	{
		return a.mStructID == b.mStructID;
	}
}
