#include "FlyDataTypeProxyIterator.hpp"
#include "../../Internal/FlyInternal.hpp"
#include "../../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{


	DataTypeProxyIterator DataTypeProxyIteratorService::begin() const
	{
		const DataTypeManager& dataTypeManager = Internal::GetDataTypeManager();
		if (!mPredicate)
		{
			return DataTypeProxyIterator(cbegin(dataTypeManager.GetDataTypes()), cend(dataTypeManager.GetDataTypes()), nullptr);
		}
		auto it = cbegin(dataTypeManager.GetDataTypes());
		auto endIt = cend(dataTypeManager.GetDataTypes());
		while (it != endIt && !mPredicate(DataTypeProxy(it->first)))
		{
			++it;
		}
		return DataTypeProxyIterator(it, cend(dataTypeManager.GetDataTypes()), &mPredicate);
	}

	DataTypeProxyIterator DataTypeProxyIteratorService::end() const
	{
		return DataTypeProxyIterator(Internal::GetDataTypeManager().GetDataTypes().end(), Internal::GetDataTypeManager().GetDataTypes().end(), mPredicate.operator bool() ? &mPredicate : nullptr);
	}
}