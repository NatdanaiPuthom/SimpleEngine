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
		[[nodiscard]] const Color& GetColor() const;
		[[nodiscard]] eDataTypeTrait GetTypeTraits() const;

		[[nodiscard]] bool IsTargetable() const;
		[[nodiscard]] bool IsPointer() const;
		[[nodiscard]] bool IsViewAndEditable() const;

		[[nodiscard]] std::optional<const std::vector<NodeTypeID>*> GetNodeTypesIDs() const;

		explicit operator bool() const;

		bool operator==(const DataTypeFacade& aOther) const;
		bool operator!=(const DataTypeFacade& aOther) const;

	private:

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