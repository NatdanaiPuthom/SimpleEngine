#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataTypeFacade.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	class ClassFacade;
	struct Variable;
	class CommandTracker;
	class StructFacade;
	class VariableContainer;

	class VariableFacade final
	{
	public:

		VariableFacade() = default;
		VariableFacade(VarID aVarID, const ClassFacade& aClassFacade);
		VariableFacade(VarID aVarID, const StructFacade& aStructFacade);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] DataTypeFacade GetDataType() const;
		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] VarID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void Destroy(CommandTracker* aCommandTracker);
		void ViewAndEditDefaultValue(CommandTracker* aCommandTracker);
		void SetDataType(DataTypeFacade aDataTypeView, CommandTracker* aCommandTracker);

		explicit operator bool() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID mVarID = InvalidID<VarID>();
		VariableContainer* mOwner = nullptr;
	};
}