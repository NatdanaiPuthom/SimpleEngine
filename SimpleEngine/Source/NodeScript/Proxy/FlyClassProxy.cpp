#include "FlyClassProxy.hpp"
#include "../DataType/FlyClass.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"
#include "../Serialization/FlySerializer.hpp"

namespace FLY_NAMESPACE
{

	ClassProxy::ClassProxy(const ClassID aClassID)
		: mClassID(aClassID)
	{
		assert(&mClassID);
	}

	std::string_view ClassProxy::GetName() const
	{
		return GetClass().mName;
	}

	DataTypeProxy ClassProxy::GetTargetDataType() const
	{
		const DataTypeID targetID = GetClass().mTargetID;
		return DataTypeProxy(targetID);
	}

	NodeGraphProxy ClassProxy::GetEventGraph() const
	{
		return NodeGraphProxy(GetClass().mEventGraph);
	}

	std::vector<VariableProxy> ClassProxy::GetVariables(const bool aIncludeDestroyed) const
	{
		const Class& flyClass = GetClass();
		std::vector<VariableProxy> variableProxys;

		variableProxys.reserve(flyClass.mVariableContainer.GetVariableCount());

		if (!aIncludeDestroyed)
		{
			for (VarID varID{ 0 }; varID < flyClass.mVariableContainer.GetVariableCount(); ++varID)
			{
				const Variable& variable = flyClass.mVariableContainer.GetVariable(varID);
				if (variable.IsDestroyed())
				{
					continue;
				}
				variableProxys.push_back(VariableProxy(varID, *this));
			}
		}
		else
		{
			for (VarID varID{ 0 }; varID < flyClass.mVariableContainer.GetVariableCount(); ++varID)
			{
				variableProxys.push_back(VariableProxy(varID, *this));
			}
		}

		return variableProxys;
	}

	std::vector<FunctionProxy> ClassProxy::GetFunctions() const
	{
		const std::vector<FunctionID>& functionIDs = GetClass().GetMemberFunctionIDs();
		std::vector<FunctionProxy> functionProxys;

		functionProxys.reserve(functionIDs.size());

		for (const FunctionID functionID : functionIDs)
		{
			functionProxys.push_back(FunctionProxy(functionID));
		}

		return functionProxys;
	}

	VariableProxy ClassProxy::CreateVariable(const GenericDataTypeProxy aDataType, const std::string_view aName, CommandTracker* const aCommandTracker)
	{
		const VarID varID = Internal::CreateVariable(GetClass().mVariableContainer, aDataType.GetID(), aName, aCommandTracker);
		return VariableProxy(varID, *this);
	}

	ClassInstanceProxy ClassProxy::CreateClassInstance()
	{
		return ClassInstanceProxy(Internal::CreateClassInstance(mClassID));
	}

	FunctionProxy ClassProxy::CreateMemberFunction(const std::string_view aName)
	{
		const FunctionID id = Internal::CreateFunction(aName);
		GetClass().BindFunction(id);

		return FunctionProxy(id);
	}

	ClassID ClassProxy::GetID() const
	{
		return mClassID;
	}

	void ClassProxy::SetName(std::string_view aName, CommandTracker* const aCommandTracker)
	{
		Internal::SetClassName(mClassID, aName, aCommandTracker);
	}

	void ClassProxy::Save(const std::string_view aSavePath) const
	{
		Internal::SaveClass(GetClass(), aSavePath);
	}

	ClassProxy::operator bool() const
	{
		return mClassID != InvalidID<ClassID>();
	}

	bool operator==(const ClassProxy& a, const ClassProxy& b)
	{
		return a.mClassID == b.mClassID;
	}

	Class& ClassProxy::GetClass() const
	{
		return Internal::GetClassByID(mClassID);
	}
}
