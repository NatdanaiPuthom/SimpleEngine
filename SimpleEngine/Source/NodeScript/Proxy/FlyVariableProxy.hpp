#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	class DataTypeProxy;
	class StructProxy;
	class ClassProxy;
	class GenericDataTypeProxy;
	class Variable;
	class CommandTracker;

	class VariableProxy final
	{
	public:

		VariableProxy() = default;
		VariableProxy(VarID aVarID, const ClassProxy& aClass);
		VariableProxy(VarID aVarID, const StructProxy& aStruct);
		VariableProxy(VarID aVarID, const DataTypeProxy& aDataType);
		VariableProxy(VarID aVarID, const GenericDataTypeProxy& aGenericDataType);

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;
		[[nodiscard]] bool IsDestroyed() const;
		[[nodiscard]] VarID GetID() const;
		[[nodiscard]] size_t GetByteOffset() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void Destroy(CommandTracker* aCommandTracker);
		void ViewAndEditDefaultValue(CommandTracker* aCommandTracker);
		void SetDataType(GenericDataTypeProxy aDataTypeProxy, CommandTracker* aCommandTracker);

		explicit operator bool() const;

	private:

		const Variable& GetVariable() const;
	private:

		VarID mVarID = InvalidID<VarID>();
		GenericDataTypeID mOwnerID;
	};
}