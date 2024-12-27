#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableContainerInstance.hpp"

namespace FLY_NAMESPACE
{

	class Struct;

	class StructInstance final
	{
	public:

		StructInstance() = default;
		StructInstance(const Struct& aStruct);


		constexpr operator bool() const
		{
			return mStruct != nullptr;
		}

		friend bool operator==(const StructInstance& a, const StructInstance& b);

		[[nodiscard]] const Struct* GetStruct() const
		{
			return mStruct;
		}

	private:

		const Struct* mStruct = nullptr;

	public:

		VariableContainerInstance mVariableContainerInstance;
	};
}