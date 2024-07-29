#pragma once
#include "FlyDefines.hpp"
#include "Variable/FlyVariableManager.hpp"
#include "FlyEventGraph.hpp"

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class Function;
	class ClassInstance;

	class Class final
	{
	public:

		Class(DataTypeID aTargetID = GetDataTypeID<None>(), const std::string& aName = "Default Script");
		~Class();

		Class(const Class&) = delete;
		Class(Class&&) = delete;
		Class& operator=(const Class&) = delete;
		Class& operator=(Class&&) = delete;

		ClassInstance& CreateClassInstance();
		void DestroyClassInstance(ClassInstance& aScriptInstance);

		void BindFunction(FunctionID aFunctionID);
		const std::vector<FunctionID>& GetMemberFunctionIDs() const;

	public:

		EventGraph mEventGraph;
		VariableManager mVariableManager;

		std::vector<FunctionID> mMemberFunctionIDs;
		std::vector<std::unique_ptr<ClassInstance>> mClassInstances;

		DataTypeID mTargetID;
		std::string mName;
	};
}

