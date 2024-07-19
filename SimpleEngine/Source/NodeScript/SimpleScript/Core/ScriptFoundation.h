#pragma once
#include "ScriptDefines.h"
#include "Memory/ScriptMemoryPool.h"
#include <memory>
#include "ScriptCallStack.h"

namespace SCR
{
	class NodeExecutor;
	class TypeManager;
	class Script;

	class ScriptFoundation final
	{
		friend class ScriptProxy;
	public:

		static ScriptFoundation& GetInstance()
		{
			if (!myInstance)
			{
				myInstance = new ScriptFoundation();
			}
			return *myInstance;
		}

		static void Destroy();
	private:
		ScriptFoundation();
		~ScriptFoundation();

	public:

		void Initialize();

		void ClearScripts();

		Script& CreateScript(const DataTypeID aTargetID, const std::string& aName = "Default Script");
		void DestroyScript(Script& aScript);
		const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Script>>>& GetScripts();

		TypeManager& GetTypeManager();
		NodeExecutor& GetNodeExecutor();

	private:

		inline static ScriptFoundation* myInstance = nullptr;

		MemoryPool myMemoryPool;
		std::unique_ptr<TypeManager> myTypeManager;
		std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Script>>> myScripts;

		std::unique_ptr<NodeExecutor> myNodeExecutor;
		CallStack myCallStack;

	};
}