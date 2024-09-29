#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataTypeFacade.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	class ClassFacade;
	struct Variable;
	class CommandTracker;

	class VariableFacade final
	{
	public:

		VariableFacade() = default;
		VariableFacade(VarID aVarID, const ClassFacade& aClassView);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] DataTypeFacade GetDataType() const;
		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] VarID GetID() const;

		[[nodiscard]] Class& GetClass() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void Destroy(CommandTracker* aCommandTracker);
		void EditDefaultValue(CommandTracker* aCommandTracker);
		void SetDataType(DataTypeFacade aDataTypeView, CommandTracker* aCommandTracker);

		explicit operator bool() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID mVarID = InvalidID<VarID>();
		Class* mClass = nullptr;
	};
}