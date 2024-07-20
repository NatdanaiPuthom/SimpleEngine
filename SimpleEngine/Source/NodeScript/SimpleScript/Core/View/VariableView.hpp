#pragma once
#include "../ScriptDefines.hpp"
#include "FlyDataTypeView.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	struct Variable;

	class VariableView final
	{
	public:

		VariableView(VarID aVarID, const Class& aClass);

		const std::string& GetName() const;
		DataTypeView GetDataType() const;
		bool IsDestroyed() const;
		VarID GetID() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID myVarID;
		const Class* myClass;
	};
}