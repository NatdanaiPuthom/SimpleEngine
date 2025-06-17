#pragma once
#include "../FlyDefines.hpp"
#include "../DataType/FlyDataTypeTrait.hpp"
#include "FlyVariableProxy.hpp"
#include <vector>
#include <optional>

namespace FLY_NAMESPACE
{

	class DataType;

	class DataTypeProxy final
	{
	public:

		DataTypeProxy() = default;
		explicit DataTypeProxy(DataTypeID aDataTypeID);

		[[nodiscard]] DataTypeID GetID() const;

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

		[[nodiscard]] std::optional<const std::vector<NodeTypeID>*> GetNodeTypesIDs() const;

		void SetColor(const Color& aColor);

		[[nodiscard]] explicit operator bool() const;

		[[nodiscard]] friend bool operator==(const DataTypeProxy& a, const DataTypeProxy& b);

	private:

		[[nodiscard]] DataType* GetDataType();
		[[nodiscard]] const DataType* GetDataType() const;

	private:

		DataTypeID mDataTypeID = InvalidID<DataTypeID>();
	};
}

template<>
struct std::hash<FLY_NAMESPACE::DataTypeProxy>
{
	size_t operator()(const FLY_NAMESPACE::DataTypeProxy& aDataTypeView) const
	{
		return aDataTypeView.GetID();
	}
};