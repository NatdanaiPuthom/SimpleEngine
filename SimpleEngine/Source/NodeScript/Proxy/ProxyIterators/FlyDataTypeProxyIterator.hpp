#pragma once
#include "../FlyDataTypeProxy.hpp"
#include "../../Utilities/FlyPredicate.hpp"
#include "../../DataType/FlyDataType.hpp"

namespace FLY_NAMESPACE
{

	class DataTypeProxyIterator final
	{
		using InternalIteratorType = std::unordered_map<DataTypeID, class DataType>::const_iterator;
	public:

		DataTypeProxyIterator(InternalIteratorType aIterator, InternalIteratorType aEndIterator, const Predicate<DataTypeProxy>* aPredicate)
			: mIterator(aIterator)
			, mEndIterator(aEndIterator)
			, mPredicate(aPredicate)
		{
		}

		DataTypeProxyIterator& operator++()
		{
			++mIterator;
			if (mPredicate)
			{
				while (mIterator != mEndIterator && !(*mPredicate)(**this))
				{
					++mIterator;
				}
			}
			return *this;
		}

		DataTypeProxy operator*()
		{
			return DataTypeProxy(mIterator->first);
		}

		friend bool operator==(const DataTypeProxyIterator& a, const DataTypeProxyIterator& b)
		{
			return a.mIterator == b.mIterator;
		}

	private:

		InternalIteratorType mIterator;
		InternalIteratorType mEndIterator;
		const Predicate<DataTypeProxy>* mPredicate;
	};


	class DataTypeProxyIteratorService
	{
	public:

		DataTypeProxyIteratorService(Predicate<DataTypeProxy> aPredicate = {})
			: mPredicate(std::move(aPredicate))
		{
		}

		DataTypeProxyIterator begin() const;

		DataTypeProxyIterator end() const;

	private:

		Predicate<DataTypeProxy> mPredicate;
	};


	template<typename IDType, typename ProxyType>
	class ProxyGlobalIterator final
	{
	public:
		using value_type = ProxyType;

		ProxyGlobalIterator(IDType aID, IDType aEndID, const Predicate<ProxyType>* aPredicate)
			: mPredicate(aPredicate)
			, mID(aID)
			, mEndID(aEndID)
		{
		}

		ProxyGlobalIterator& operator++()
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
			return ProxyType(mID);
		}

		friend bool operator==(const ProxyGlobalIterator& a, const ProxyGlobalIterator& b)
		{
			return a.mID == b.mID;
		}

	private:

		const Predicate<ProxyType>* mPredicate = nullptr;
		IDType mID;
		IDType mEndID;
	};

	template<typename IDType, typename ProxyIteratorType, typename ProxyType = ProxyIteratorType::value_type>
	class ProxyGlobalIteratorService final
	{
	public:

		ProxyGlobalIteratorService(IDType aEndID, Predicate<ProxyType> aPredicate = Predicate<ProxyType>())
			: mEndID(aEndID)
			, mPredicate(std::move(aPredicate))
		{
		}

		ProxyIteratorType begin() const
		{
			if (!mPredicate)
			{
				return ProxyIteratorType(IDType{ 0 }, mEndID, nullptr);
			}
			IDType firstID{ 0 };
			while (firstID != mEndID && !mPredicate(ProxyType(firstID)))
			{
				++firstID;
			}
			return ProxyIteratorType(firstID, mEndID, &mPredicate);
		}

		ProxyIteratorType end() const
		{
			return ProxyIteratorType(mEndID, mEndID, mPredicate.operator bool() ? &mPredicate : nullptr);
		}

	private:

		Predicate<ProxyType> mPredicate;
		IDType mEndID{ 0 };
	};

}