#pragma once
#include "../FlyDefines.hpp"
#include <vector>
#include "FlyVariable.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	template<size_t> class MemoryArena;

	class VariableManager final
	{
		friend class ScriptProxy;
	public:

		VariableManager();
		~VariableManager();

		VariableManager(const VariableManager&) = delete;
		VariableManager(VariableManager&&) = default;
		VariableManager& operator=(const VariableManager&) = delete;
		VariableManager& operator=(VariableManager&&) = default;

	public:

		std::vector<Variable> mVariables;

		MemoryArena<NodeBufferCapacity> mMemoryArena;

	};
	
}