#include "FlyDataType.hpp"

namespace FLY_NAMESPACE
{
	DataType::DataType(std::string aName, size_t aSize, size_t aAlignment, Color aColor, const std::type_info* aTypeInfo, DataTypeInterface aInterface, DataTypeID aToPointerDataTypeID, DataTypeID aToValueDataTypeID, eDataTypeTrait aTraits)
		: mName(std::move(aName))
		, mSize(aSize)
		, mAlignment(aAlignment)
		, mColor(aColor)
		, mTypeInfo(aTypeInfo)
		, mInterface(aInterface)
		, mToPointerDataTypeID(aToPointerDataTypeID)
		, mToValueDataTypeID(aToValueDataTypeID)
		, mTypeTraits(aTraits)
	{
	}

	const std::type_info* DataType::GetTypeInfo() const
	{
		return mTypeInfo;
	}

	const std::vector<NodeTypeID>& DataType::GetMemberNodeTypeIDs() const
	{
		return mMemberNodeTypeIDs;
	}

	NodeTypeID DataType::GetBreakerNodeTypeID() const
	{
		return mBreakerNodeTypeID;
	}

	NodeTypeID DataType::GetMakerNodeTypeID() const
	{
		return mMakerNodeTypeID;
	}

	void DataType::SetColor(Color aColor)
	{
		mColor = aColor;
	}

	void DataType::SetBreakerNodeTypeID(NodeTypeID aNodeTypeID)
	{
		mBreakerNodeTypeID = aNodeTypeID;
	}

	void DataType::SetMakerNodeTypeID(NodeTypeID aNodeTypeID)
	{
		mMakerNodeTypeID = aNodeTypeID;
	}

	void DataType::AddMemberNodeTypeID(NodeTypeID aNodeTypeID)
	{
		mMemberNodeTypeIDs.push_back(aNodeTypeID);
	}
}