#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
{
	class Variable final
	{
	public:

		Variable() = default;
		Variable(GenericDataTypeID aDataTypeID, std::string aName, size_t aByteOffset);

		void SetDataTypeID(GenericDataTypeID aDataTypeID);
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;
		void SetDefaultValueDataPtr(void* aDataPtr);
		[[nodiscard]] OwningPtr<void> GetDefaultValueDataPtr() const;
		[[nodiscard]] std::string& Name();
		[[nodiscard]] const std::string& Name() const;

		[[nodiscard]] size_t GetByteOffset() const;
		void SetIsDestroyed(bool aIsDestroyed);
		[[nodiscard]] bool IsDestroyed() const;

	private:

		GenericDataTypeID mDataTypeID;
		OwningPtr<void> mDefaultValueDataPtr;
		std::string mName = "Var";
		size_t mByteOffset = 0;
		bool mIsDestroyed = false;
	};

	inline GenericDataTypeID Variable::GetDataTypeID() const
	{
		return mDataTypeID;
	}

	inline void Variable::SetDefaultValueDataPtr(void* aDataPtr)
	{
		mDefaultValueDataPtr = aDataPtr;
	}

	inline OwningPtr<void> Variable::GetDefaultValueDataPtr() const
	{
		return mDefaultValueDataPtr;
	}

	inline size_t Variable::GetByteOffset() const
	{
		return mByteOffset;
	}
}