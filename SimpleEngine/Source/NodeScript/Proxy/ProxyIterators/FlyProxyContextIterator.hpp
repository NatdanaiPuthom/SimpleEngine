#pragma once
#include "../../Graph/FlyNodeGraphVariant.hpp"
#include "../../Utilities/FlyPredicate.hpp"

namespace FLY_NAMESPACE
{

	template<typename IDType, typename ProxyType>
	class ProxyContextIterator final
	{
	public:
		using value_type = ProxyType;

		ProxyContextIterator(IDType aID, const NodeGraphVariantHandle& aNodeGraphVariantHandle, IDType aEndID, const Predicate<ProxyType>* aPredicate)
			: mNodeGraphVariantHandle(aNodeGraphVariantHandle)
			, mPredicate(aPredicate)
			, mID(aID)
			, mEndID(aEndID)
		{
		}

		ProxyContextIterator& operator++()
		{
			++mID;
			if (mPredicate)
			{
				while (mID != mEndID && !(*mPredicate)(**this))
				{
					++mID;
				}
			}
			return *this;
		}

		ProxyType operator*() const
		{
			return ProxyType(mID, mNodeGraphVariantHandle);
		}

		friend bool operator==(const ProxyContextIterator& a, const ProxyContextIterator& b)
		{
			return a.mNodeGraphVariantHandle == b.mNodeGraphVariantHandle && a.mID == b.mID;
		}

	private:

		NodeGraphVariantHandle mNodeGraphVariantHandle;
		const Predicate<ProxyType>* mPredicate = nullptr;
		IDType mID;
		IDType mEndID;
	};

	template<typename IDType, typename ProxyIteratorType, typename ProxyType = ProxyIteratorType::value_type>
	class ProxyContextIteratorService final
	{
	public:

		ProxyContextIteratorService(const NodeGraphVariantHandle& aNodeGraphVariantHandle, IDType aEndID, Predicate<ProxyType> aPredicate = Predicate<ProxyType>())
			: mNodeGraphVariantHandle(aNodeGraphVariantHandle)
			, mEndID(aEndID)
			, mPredicate(std::move(aPredicate))
		{
		}

		ProxyIteratorType begin() const
		{
			IDType firstID{ 0 };
			while (firstID != mEndID && !mPredicate(ProxyType(firstID, mNodeGraphVariantHandle)))
			{
				++firstID;
			}
			return ProxyIteratorType(firstID, mNodeGraphVariantHandle, mEndID, mPredicate.operator bool() ? &mPredicate : nullptr);
		}

		ProxyIteratorType end() const
		{
			return ProxyIteratorType(mEndID, mNodeGraphVariantHandle, mEndID, mPredicate.operator bool() ? &mPredicate : nullptr);
		}

	private:

		NodeGraphVariantHandle mNodeGraphVariantHandle;
		Predicate<ProxyType> mPredicate;
		IDType mEndID{ 0 };
	};
}