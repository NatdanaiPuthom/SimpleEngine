#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataTypeView.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	struct Variable;

	class VariableView final
	{
	public:

		VariableView() = default;
		VariableView(VarID aVarID, const Class& aClass);

		const std::string& GetName() const;
		DataTypeView GetDataType() const;
		bool IsDestroyed() const;
		VarID GetID() const;

		explicit operator bool() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID mVarID = InvalidID<VarID>();
		const Class* mClass = nullptr;
	};
}