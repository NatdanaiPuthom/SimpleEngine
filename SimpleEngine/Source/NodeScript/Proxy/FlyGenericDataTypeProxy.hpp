#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableProxy.hpp"
#include "../DataType/FlyGenericDataType.hpp"
#include "../DataType/FlyDataTypeTrait.hpp"

namespace FLY_NAMESPACE
{

	class DataTypeProxy;
	class DataType;

	class GenericDataTypeProxy final
	{
	public:

		GenericDataTypeProxy() = default;
		explicit GenericDataTypeProxy(GenericDataTypeID aDataTypeID);
		explicit GenericDataTypeProxy(const DataTypeProxy& aDataType);
		explicit GenericDataTypeProxy(DataTypeID aDataTypeID);
		explicit GenericDataTypeProxy(ClassID aClassID);

		[[nodiscard]] GenericDataTypeID GetID() const;

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] Color GetColor() const;
		[[nodiscard]] eDataTypeTrait GetTypeTraits() const;
		[[nodiscard]] size_t GetSize() const;
		[[nodiscard]] size_t GetAlignment() const;

		[[nodiscard]] bool IsTargetable() const;
		[[nodiscard]] bool IsPointer() const;
		[[nodiscard]] bool IsViewAndEditable() const;
		[[nodiscard]] bool IsViewable() const;
		[[nodiscard]] std::vector<VariableProxy> GetMemberVariables() const;

		void SetColor(const Color& aColor);
		VariableProxy CreateMemberVariable(GenericDataTypeProxy aDataTypeProxy, std::string_view aName, CommandTracker* const aCommandTracker);

		[[nodiscard]] explicit operator bool() const;

		[[nodiscard]] bool operator==(const GenericDataTypeProxy& aOther) const;

	private:

		[[nodiscard]] DataType* GetDataType() const;

	private:

		GenericDataTypeID mDataTypeID;
	};

}