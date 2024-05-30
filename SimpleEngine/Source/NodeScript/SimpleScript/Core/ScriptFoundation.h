#pragma once
#include "ScriptDefines.h"
#include "Memory/ScriptMemoryPool.h"
#include <memory>
#include "ScriptManager.h"
#include "ScriptCallStack.h"

namespace SCR
{
	class ScriptManager;
	class Function;
	
	class ScriptFoundation final
	{
		friend class ScriptProxy;
	public:

		ScriptFoundation();
		~ScriptFoundation();

		void InitializeSystemTypes();

		ScriptManager& CreateScriptManager();
		void DestroyScriptManager(ScriptManager& aScriptManager);

		void Clear();

	private:

		static MemoryPool myGlobalMemoryPool;

		std::vector<std::unique_ptr<ScriptManager>> myScriptManagers;

		std::vector<std::unique_ptr<Function>> myGlobalFunctions;

		CallStack myCallStack;

	};
}