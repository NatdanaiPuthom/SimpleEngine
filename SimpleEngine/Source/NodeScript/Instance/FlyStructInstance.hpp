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

	private:

		const Struct* mStruct = nullptr;

		VariableContainerInstance mVariableContainerInstance;
	};
}