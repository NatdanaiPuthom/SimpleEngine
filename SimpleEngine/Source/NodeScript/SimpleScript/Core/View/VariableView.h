#pragma once
#include "../ScriptDefines.h"
#include "../Variable/VariableManager.h"

namespace SCR
{

	class Script;


	class VariableView final
	{
	public:

		VariableView(VarID aVarID, const Script& aScript);

		const std::string& GetName() const;
		DataTypeID GetDataTypeID() const;
		bool IsDestroyed() const;
		VarID GetID() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID myVarID;
		const Script* myScript;
	};
}