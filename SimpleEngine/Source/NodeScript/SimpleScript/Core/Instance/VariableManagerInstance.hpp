#pragma once
#include "../ScriptDefines.hpp"

namespace FLY_NAMESPACE
{

	struct VariableInstance
	{
		void* mRuntimeDataPtr = nullptr;
	};

	class VariableManager;
	template<size_t> class MemoryArena;

	constexpr size_t VariableArenaSize = 1024;

	class VariableManagerInstance final
	{
	public:

		VariableManagerInstance();
		~VariableManagerInstance();

		VariableManagerInstance(const VariableManagerInstance& aOther);
		VariableManagerInstance(VariableManagerInstance&&) noexcept = default;
		VariableManagerInstance& operator=(const VariableManagerInstance& aOther);
		VariableManagerInstance& operator=(VariableManagerInstance&&) noexcept = default;

		void Init(VariableManager& aVariableManager);

	
		std::vector<VariableInstance> mVariables;
		std::unique_ptr<MemoryArena<VariableArenaSize>> mMemoryArena;
	};
}