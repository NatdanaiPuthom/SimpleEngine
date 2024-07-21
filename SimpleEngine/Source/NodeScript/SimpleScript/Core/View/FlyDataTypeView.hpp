#pragma once
#include "../ScriptDefines.hpp"
#include "NodeTypeView.hpp"
#include "../DataType/FlyDataType.hpp"

namespace FLY_NAMESPACE
{

	struct DataType;

	class DataTypeView final
	{
	public:

		DataTypeView();
		explicit DataTypeView(DataTypeID aDataTypeID);

		DataTypeID GetID() const;

		const std::string& GetName() const;
		const Color& GetColor() const;
		eDataTypeTrait GetTypeTraits() const;

		bool IsTargetable() const;
		bool IsEditable() const;

		std::vector<NodeTypeView> GetNodeTypes() const;

		explicit operator bool() const;

		bool operator==(const DataTypeView& aOther) const;
		bool operator!=(const DataTypeView& aOther) const;

	private:

		const DataType& GetDataType() const;

	private:

		DataTypeID mDataTypeID = InvalidID<DataTypeID>();
	};
}

template<>
struct std::hash<FLY_NAMESPACE::DataTypeView>
{
	size_t operator()(const FLY_NAMESPACE::DataTypeView& aDataTypeView) const
	{
		return aDataTypeView.GetID();
	}
};