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
	struct CopyBuffer;
	class NodeExecutor;
	class CommandTracker;
	
	class ScriptFoundation final
	{
		friend class ScriptProxy;
	public:

		static ScriptFoundation& GetInstance()
		{
			return *myFoundationPtr;
		}

		ScriptFoundation();
		~ScriptFoundation();

		void InitializeSystemTypes();

		ScriptManager& CreateScriptManager();
		void DestroyScriptManager(ScriptManager& aScriptManager);

		void Clear();

	private:

		static MemoryPool myGlobalMemoryPool;
		inline static ScriptFoundation* myFoundationPtr = nullptr;

		std::vector<std::unique_ptr<ScriptManager>> myScriptManagers;

		std::vector<std::unique_ptr<Function>> myGlobalFunctions;

		CallStack myCallStack;

		std::unique_ptr<CopyBuffer> myCopyBuffer;

		std::unique_ptr<CommandTracker> myCommandTracker;

		std::unique_ptr<NodeExecutor> myNodeExecutor;
	};
}