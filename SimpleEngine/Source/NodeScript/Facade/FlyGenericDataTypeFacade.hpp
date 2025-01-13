#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableFacade.hpp"
#include "../DataType/FlyGenericDataType.hpp"
#include "../DataType/FlyDataTypeTrait.hpp"

namespace FLY_NAMESPACE
{

	class DataTypeFacade;

	class GenericDataTypeFacade final
	{
	public:

		GenericDataTypeFacade() = default;
		explicit GenericDataTypeFacade(GenericDataTypeID aDataTypeID);
		explicit GenericDataTypeFacade(const DataTypeFacade& aDataTypeFacade);
		explicit GenericDataTypeFacade(DataTypeID aDataTypeID);
		explicit GenericDataTypeFacade(StructID aStructID);
		explicit GenericDataTypeFacade(ClassID aClassID);

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
		[[nodiscard]] std::vector<VariableFacade> GetMemberVariables() const;

		void SetColor(const Color& aColor);

		[[nodiscard]] explicit operator bool() const;

		[[nodiscard]] bool operator==(const GenericDataTypeFacade& aOther) const;

	private:

		[[nodiscard]] DataType* GetDataType() const;

	private:

		GenericDataTypeID mDataTypeID;
	};

}