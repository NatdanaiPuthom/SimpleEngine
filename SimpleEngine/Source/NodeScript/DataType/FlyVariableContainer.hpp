#pragma once
#include "../FlyDefines.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include "FlyVariable.hpp"
#include <vector>

namespace FLY_NAMESPACE
{
	class VariableContainer final
	{
		static constexpr size_t Alignment = 512;
	public:

		VariableContainer() = default;
		~VariableContainer() = default;

		VariableContainer(const VariableContainer& aOther);
		VariableContainer(VariableContainer&&) = default;
		VariableContainer& operator=(const VariableContainer& aOther);
		VariableContainer& operator=(VariableContainer&&) = default;

	public:

		std::vector<Variable> mVariables;

		MemoryArena<Alignment> mMemoryArena;

	};
}