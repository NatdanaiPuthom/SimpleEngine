#pragma once
#include "../../Graph/FlyNodeGraphVariant.hpp"

namespace FLY_NAMESPACE
{
	template<typename IDType, typename ProxyType>
	class ProxyIterator final
	{
	public:
		ProxyIterator(IDType aID, const NodeGraphVariantHandle& aNodeGraphVariantHandle)
			: mNodeGraphVariantHandle(aNodeGraphVariantHandle)
			, mID(aID)
		{
		}

		ProxyIterator operator++(int)
		{
			mID++;
			return ProxyIterator(*this);
		}

		ProxyIterator& operator++()
		{
			++mID;
			return *this;
		}

		ProxyType operator*() const
		{
			return ProxyType(mID, mNodeGraphVariantHandle);
		}

		friend bool operator==(const ProxyIterator& a, const ProxyIterator& b)
		{
			return a.mNodeGraphVariantHandle == b.mNodeGraphVariantHandle && a.mID == b.mID;
		}

	private:

		NodeGraphVariantHandle mNodeGraphVariantHandle;
		IDType mID;

	};

	template<typename IDType, typename ProxyIteratorType>
	class ProxyIteratorService final
	{
		using FindFunctionType = IDType(const NodeGraphVariantHandle&);
	public:

		ProxyIteratorService(const NodeGraphVariantHandle& aNodeGraphVariantHandle, bool aIncludeDestroyed, FindFunctionType* aFindFirstFunction, FindFunctionType* aFindLastFunction)
			: mNodeGraphVariantHandle(aNodeGraphVariantHandle)
			, mIncludeDestroyed(aIncludeDestroyed)
			, mFindFirstIDFunction(aFindFirstFunction)
			, mFindLastIDFunction(aFindLastFunction)
		{
		}

		ProxyIteratorType begin() const
		{
			if (mIncludeDestroyed)
			{
				return ProxyIteratorType(IDType{ 0 }, mNodeGraphVariantHandle);
			}
			return ProxyIteratorType(mFindFirstIDFunction(mNodeGraphVariantHandle), mNodeGraphVariantHandle);
		}

		ProxyIteratorType end() const
		{
			return ProxyIteratorType(mFindLastIDFunction(mNodeGraphVariantHandle), mNodeGraphVariantHandle);
		}

	private:

		NodeGraphVariantHandle mNodeGraphVariantHandle;
		bool mIncludeDestroyed = false;

		FindFunctionType* mFindFirstIDFunction = nullptr;
		FindFunctionType* mFindLastIDFunction = nullptr;
	};
}