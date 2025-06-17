#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	class DataType;
	class Variable;

	class MemberVariableProxy final
	{
	public:

		MemberVariableProxy() = default;
		MemberVariableProxy(DataTypeID aParentDataTypeID, size_t aMemberIndex);

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;
		[[nodiscard]] size_t GetByteOffset() const;

	private:

		[[nodiscard]] const DataType& GetParentDataType() const;
		[[nodiscard]] const Variable& GetVariable() const;

	private:

		DataTypeID mParentDataTypeID = InvalidID<DataTypeID>();
		size_t mMemberIndex = InvalidID<size_t>();
	};
}