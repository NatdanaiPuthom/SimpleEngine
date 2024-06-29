#pragma once
#include "../ScriptDefines.h"
#include "../Memory/ScriptMemoryManager.h"

namespace SCR
{

	struct VariableInstance
	{
		void* runtimeDataPtr = nullptr;
	};

	class VariableManager;

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

	
		std::vector<VariableInstance> myVariables;
		std::unique_ptr<MemoryManager> myMemoryManager;
	};
}