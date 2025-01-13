#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	class DataTypeFacade;
	class StructFacade;
	class ClassFacade;
	class GenericDataTypeFacade;
	struct Variable;
	class CommandTracker;

	class VariableFacade final
	{
	public:

		VariableFacade() = default;
		VariableFacade(VarID aVarID, const ClassFacade& aClassFacade);
		VariableFacade(VarID aVarID, const StructFacade& aStructFacade);
		VariableFacade(VarID aVarID, const DataTypeFacade& aDataTypeFacade);
		VariableFacade(VarID aVarID, const GenericDataTypeFacade& aGenericDataTypeFacade);

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;
		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] VarID GetID() const;
		[[nodiscard]] size_t GetByteOffset() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void Destroy(CommandTracker* aCommandTracker);
		void ViewAndEditDefaultValue(CommandTracker* aCommandTracker);
		void SetDataType(GenericDataTypeFacade aDataTypeFacade, CommandTracker* aCommandTracker);

		explicit operator bool() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID mVarID = InvalidID<VarID>();
		GenericDataTypeID mOwnerID;
	};
}