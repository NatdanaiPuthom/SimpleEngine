#pragma once
#include "../FlyDefines.hpp"
#include "../DataType/FlyDataType.hpp"

namespace FLY_NAMESPACE
{

	struct DataType;

	class DataTypeFacade final
	{
	public:

		DataTypeFacade();
		explicit DataTypeFacade(DataTypeID aDataTypeID);

		[[nodiscard]] DataTypeID GetID() const;

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] Color GetColor() const;
		[[nodiscard]] eDataTypeTrait GetTypeTraits() const;

		[[nodiscard]] bool IsTargetable() const;
		[[nodiscard]] bool IsPointer() const;
		[[nodiscard]] bool IsViewAndEditable() const;
		[[nodiscard]] bool IsViewable() const;

		[[nodiscard]] std::optional<const std::vector<NodeTypeID>*> GetNodeTypesIDs() const;

		void SetColor(const Color& aColor);

		explicit operator bool() const;

		bool operator==(const DataTypeFacade& aOther) const;
		bool operator!=(const DataTypeFacade& aOther) const;

	private:

		[[nodiscard]] DataType* GetDataType();
		[[nodiscard]] const DataType* GetDataType() const;

	private:

		DataTypeID mDataTypeID = InvalidID<DataTypeID>();
	};
}

template<>
struct std::hash<FLY_NAMESPACE::DataTypeFacade>
{
	size_t operator()(const FLY_NAMESPACE::DataTypeFacade& aDataTypeView) const
	{
		return aDataTypeView.GetID();
	}
};