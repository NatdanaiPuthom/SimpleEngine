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
	class TypeManager;

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

		void Destroy()
		{
			if (myInstance)
			{

				delete myInstance;
				myInstance = nullptr;
			}
		}
	private:
		ScriptFoundation();
		~ScriptFoundation();

	public:

		void InitializeSystemTypes();

		void ClearScripts();

		Script& CreateScript(const DataTypeID aTargetID, const std::string& aName = "Default Script");
		void DestroyScript(Script& aScript);
		const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Script>>>& GetScripts();

		TypeManager& GetTypeManager();

	private:

		inline static ScriptFoundation* myInstance = nullptr;

		MemoryPool myMemoryPool;
		std::unique_ptr<TypeManager> myTypeManager;

		std::unique_ptr<NodeExecutor> myNodeExecutor;

		std::vector<std::unique_ptr<Function>> myGlobalFunctions;

		std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Script>>> myScripts;

		CallStack myCallStack;

		std::unique_ptr<CopyBuffer> myCopyBuffer;

	};
}