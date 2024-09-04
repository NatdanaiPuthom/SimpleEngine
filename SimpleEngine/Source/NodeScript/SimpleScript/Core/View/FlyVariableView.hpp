#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataTypeView.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	class ClassView;
	struct Variable;
	class CommandTracker;

	class VariableView final
	{
	public:

		VariableView() = default;
		VariableView(VarID aVarID, const ClassView& aClassView);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] DataTypeView GetDataType() const;
		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] VarID GetID() const;

		[[nodiscard]] Class& GetClass() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void Destroy(CommandTracker* aCommandTracker);
		void EditDefaultValue(CommandTracker* aCommandTracker);
		void SetDataType(DataTypeView aDataTypeView, CommandTracker* aCommandTracker);

		explicit operator bool() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID mVarID = InvalidID<VarID>();
		Class* mClass = nullptr;
	};
}