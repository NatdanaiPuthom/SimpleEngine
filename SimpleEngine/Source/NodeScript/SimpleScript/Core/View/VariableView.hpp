#pragma once
#include "../ScriptDefines.hpp"
#include "../Variable/VariableManager.hpp"

namespace SCR
{

	class Class;


	class VariableView final
	{
	public:

		VariableView(VarID aVarID, const Class& aClass);

		const std::string& GetName() const;
		DataTypeID GetDataTypeID() const;
		bool IsDestroyed() const;
		VarID GetID() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID myVarID;
		const Class* myClass;
	};
}