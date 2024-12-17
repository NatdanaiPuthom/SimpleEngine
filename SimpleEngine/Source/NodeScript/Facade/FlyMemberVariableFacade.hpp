#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	struct DataType;
	struct Variable;

	class MemberVariableFacade final
	{
	public:

		MemberVariableFacade() = default;
		MemberVariableFacade(DataTypeID aParentDataTypeID, size_t aMemberIndex);

		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] DataTypeID GetDataTypeID() const;
		[[nodiscard]] size_t GetByteOffset() const;

	private:

		[[nodiscard]] const DataType& GetParentDataType() const;
		[[nodiscard]] const Variable& GetVariable() const;

	private:

		DataTypeID mParentDataTypeID = InvalidID<DataTypeID>();
		size_t mMemberIndex = InvalidID<size_t>();
	};
}