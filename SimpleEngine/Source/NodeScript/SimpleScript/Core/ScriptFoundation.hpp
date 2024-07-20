#pragma once
#include "ScriptDefines.hpp"
#include "Memory/ScriptMemoryArena.hpp"
#include <memory>

namespace SCR
{
	class NodeExecutor;
	class TypeManager;
	class Class;

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

		Class& CreateClass(const DataTypeID aTargetID, const std::string& aName = "Default Script");
		void DestroyScript(Class& aScript);
		const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& GetScripts();

		TypeManager& GetTypeManager();
		NodeExecutor& GetNodeExecutor();

	private:

		inline static ScriptFoundation* myInstance = nullptr;

		MemoryPool myMemoryPool;
		std::unique_ptr<TypeManager> myTypeManager;
		std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>> myScripts;

		std::unique_ptr<NodeExecutor> myNodeExecutor;

	};
}