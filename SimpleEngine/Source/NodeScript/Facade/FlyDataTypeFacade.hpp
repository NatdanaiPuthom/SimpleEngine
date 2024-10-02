#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeTypeFacade.hpp"
#include "../DataType/FlyDataType.hpp"

namespace FLY_NAMESPACE
{

	struct DataType;

	class DataTypeFacade final
	{
	public:

		DataTypeFacade();
		explicit DataTypeFacade(DataTypeID aDataTypeID);

		DataTypeID GetID() const;

		const std::string& GetName() const;
		const Color& GetColor() const;
		eDataTypeTrait GetTypeTraits() const;

		bool IsTargetable() const;
		bool IsEditable() const;

		std::vector<NodeTypeFacade> GetNodeTypes() const;

		explicit operator bool() const;

		bool operator==(const DataTypeFacade& aOther) const;
		bool operator!=(const DataTypeFacade& aOther) const;

	private:

		const DataType* GetDataType() const;

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