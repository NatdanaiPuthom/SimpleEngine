#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableContainer.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "../Memory/FlyHeapObject.hpp"

namespace FLY_NAMESPACE
{

	class CommandTracker;
	class Function;
	class ClassInstance;

	class Class final
	{
	public:

		Class(DataTypeID aTargetID = GetDataTypeID<None>(), const std::string& aName = "Default Class");
		~Class();

		Class(const Class&) = delete;
		Class(Class&&) = delete;
		Class& operator=(const Class&) = delete;
		Class& operator=(Class&&) = delete;

		void BindFunction(FunctionID aFunctionID);
		const std::vector<FunctionID>& GetMemberFunctionIDs() const;

	public:

		EventGraph mEventGraph;
		VariableContainer mVariableContainer;

		std::vector<FunctionID> mMemberFunctionIDs;
		std::vector<HeapObject<ClassInstance>> mClassInstances;

		DataTypeID mTargetID;
		std::string mName;
	};
}

