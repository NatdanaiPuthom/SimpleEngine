#include "FlyStructProxy.hpp"
#include "../DataType/FlyStruct.hpp"
#include "../Internal/FlyInternal.hpp"
#include "FlyDataTypeProxy.hpp"
#include "FlyGenericDataTypeProxy.hpp"

namespace FLY_NAMESPACE
{

	

	StructProxy::StructProxy(const StructID aStructID)
		: mStructID(aStructID)
	{
	}

	std::string_view StructProxy::GetName() const
	{
		return GetStruct().Name();
	}

	StructID StructProxy::GetID() const
	{
		return mStructID;
	}

	std::vector<VariableProxy> StructProxy::GetMemberVariables(const bool aIncludeDestroyed) const
	{
		std::vector<VariableProxy> variableProxys;

		Struct& s = GetStruct();

		variableProxys.reserve(s.GetVariableContainer().GetVariables().size());
		for (size_t i = 0; i < s.GetVariableContainer().GetVariableCount(); i++)
		{
			if (!aIncludeDestroyed && s.GetVariableContainer().GetVariable(i).IsDestroyed())
			{
				continue;
			}
			variableProxys.push_back(VariableProxy(VarID{ i }, *this));
		}

		return variableProxys;

	}

	VariableProxy StructProxy::CreateVariable(const GenericDataTypeProxy& aDataTypeProxy, const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		VarID varID = Internal::CreateVariable(GetStruct().GetVariableContainer(), aDataTypeProxy.GetID(), aName, aCommandTracker);
		return VariableProxy(varID, *this);
	}

	void StructProxy::SetName(std::string_view aName, CommandTracker* const aCommandTracker)
	{
		aName;
		aCommandTracker;
		//Internal::SetStructName(mStructID, aName, aCommandTracker);
	}

	Struct& StructProxy::GetStruct() const
	{
		static Struct s;
		return s;
	}

	StructProxy::operator bool() const
	{
		return mStructID != InvalidID<StructID>();
	}

	bool operator==(const StructProxy& a, const StructProxy& b)
	{
		return a.mStructID == b.mStructID;
	}
}
