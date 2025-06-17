#pragma once
#include "../FlyDefines.hpp"
#include "nlohmann/json.hpp"

namespace FLY_NAMESPACE
{
	using InplaceAllocateF = FunctionWrapper<void(void*, const void*), struct InplaceAllocateParam>;
	using ReleaseF = FunctionWrapper<void(void*), struct ReleaseParam>;;
	using CopyF = FunctionWrapper<void(void*, const void*), struct CopyParam>;
	using SwapF = FunctionWrapper<void(void*, void*), struct SwapParam>;
	using EqualsF = FunctionWrapper<bool(const void*, const void*), struct EqualsParam>;

	using InplaceAllocateInterface = void(*)(void* aDataPtr, const void* aDefaultValuePtr);
	using ReleaseInterface = void(*)(void* aDataPtr);
	using CopyInterface = void(*)(void* aDestination, const void* aSource);
	using SwapInterface = void(*)(void* aDataPtr1, void* aDataPtr2);
	using EqualsInterface = bool(*)(const void* aDataPtr1, const void* aDataPtr2);

	struct FundamentalInterface final
	{
		InplaceAllocateF mInplaceAllocateF;
		ReleaseF mReleaseF;
		CopyF mCopyF;
		SwapF mSwapF;
		EqualsF mEqualsF;
	};

	using EditorTextFunction = void(*)(const std::string& aText);

	using ViewAndEditF = FunctionWrapper<ViewAndEditResult(void* aDataPtr, EditorTextFunction aNullptrFunction), struct ViewAndEditParam>;
	using ViewF = FunctionWrapper<void(const void* aDataPtr, EditorTextFunction aNullptrFunction), struct ViewParam>;
	using SaveF = FunctionWrapper<void(const void* aDataPtr, nlohmann::json& aSaveObject), struct SaveParam>;
	using LoadF = FunctionWrapper<void(void* aDataPtr, const nlohmann::json& aLoadObject), struct LoadParam>;

	struct FunctionInterface final
	{
		ViewAndEditF mViewAndEditF;
		ViewF mViewF;
		SaveF mSaveF;
		LoadF mLoadF;
	};

	struct ExecutionInterface final
	{
		SetPinValueF mSetInputPinValueF;
		SetPinValueF mSetOutputPinValueF;
		SetPinValueFromPinF mSetInputPinValueFromPinF;
		SetPinValueFromPinF mSetOutputPinValueFromPinF;
	};

	class DataTypeInterface final
	{
	public:

		DataTypeInterface() = default;
		DataTypeInterface(FundamentalInterface aFundamentalInterface, FunctionInterface aFunctionInterface, ExecutionInterface aExecutionInterface);

		[[nodiscard]] InplaceAllocateF GetInplaceAllocateFunction() const;
		[[nodiscard]] ReleaseF GetReleaseFunction() const;
		[[nodiscard]] CopyF GetCopyFunction() const;
		[[nodiscard]] SwapF GetSwapFunction() const;
		[[nodiscard]] EqualsF GetEqualsFunction() const;

		[[nodiscard]] ViewAndEditF GetViewAndEditFunction() const;
		[[nodiscard]] ViewF GetViewFunction() const;
		[[nodiscard]] SaveF GetSaveFunction() const;
		[[nodiscard]] LoadF GetLoadFunction() const;

		[[nodiscard]] SetPinValueF GetSetInputPinValueFunction() const;
		[[nodiscard]] SetPinValueF GetSetOutputPinValueFunction() const;
		[[nodiscard]] SetPinValueFromPinF GetSetInputPinValueFromPinFunction() const;
		[[nodiscard]] SetPinValueFromPinF GetSetOutputPinValueFromPinFunction() const;

	private:

		FundamentalInterface mFundamental;
		FunctionInterface mFunction;
		ExecutionInterface mExecution;
	};

	inline InplaceAllocateF DataTypeInterface::GetInplaceAllocateFunction() const
	{
		return mFundamental.mInplaceAllocateF;
	}

	inline ReleaseF DataTypeInterface::GetReleaseFunction() const
	{
		return mFundamental.mReleaseF;
	}

	inline CopyF DataTypeInterface::GetCopyFunction() const
	{
		return mFundamental.mCopyF;
	}

	inline SwapF DataTypeInterface::GetSwapFunction() const
	{
		return mFundamental.mSwapF;
	}

	inline EqualsF DataTypeInterface::GetEqualsFunction() const
	{
		return mFundamental.mEqualsF;
	}

	inline ViewAndEditF DataTypeInterface::GetViewAndEditFunction() const
	{
		return mFunction.mViewAndEditF;
	}

	inline ViewF DataTypeInterface::GetViewFunction() const
	{
		return mFunction.mViewF;
	}

	inline SaveF DataTypeInterface::GetSaveFunction() const
	{
		return mFunction.mSaveF;
	}

	inline LoadF DataTypeInterface::GetLoadFunction() const
	{
		return mFunction.mLoadF;
	}

	inline SetPinValueF DataTypeInterface::GetSetInputPinValueFunction() const
	{
		return mExecution.mSetInputPinValueF;
	}

	inline SetPinValueF DataTypeInterface::GetSetOutputPinValueFunction() const
	{
		return mExecution.mSetOutputPinValueF;
	}

	inline SetPinValueFromPinF DataTypeInterface::GetSetInputPinValueFromPinFunction() const
	{
		return mExecution.mSetInputPinValueFromPinF;
	}

	inline SetPinValueFromPinF DataTypeInterface::GetSetOutputPinValueFromPinFunction() const
	{
		return mExecution.mSetOutputPinValueFromPinF;
	}
}