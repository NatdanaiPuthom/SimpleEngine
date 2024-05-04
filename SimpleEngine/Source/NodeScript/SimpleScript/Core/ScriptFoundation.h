#pragma once
#include "ScriptDefines.h"
#include "Memory/ScriptMemoryPool.h"
#include <memory>
#include "ScriptManager.h"

namespace SCR
{

	class ScriptManager;
	
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

		inline static MemoryPool myGlobalMemoryPool;

		std::vector<std::unique_ptr<ScriptManager>> myScriptManagers;

	};
}