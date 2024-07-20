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
		DataTypeView(DataTypeID aDataTypeID);

		DataTypeID GetID() const;

		const std::string& GetName() const;
		const Color& GetColor() const;
		eDataTypeTrait GetTypeTraits() const;

		bool IsTargetable() const;

		std::vector<NodeTypeView> GetNodeTypes() const;

		operator bool() const;

		bool operator==(const DataTypeView& aOther) const;
		bool operator!=(const DataTypeView& aOther) const;

	private:

		const DataType& GetDataType() const;

	private:

		DataTypeID mDataTypeID = InvalidID<DataTypeID>();
	};
}