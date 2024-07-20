#pragma once
#include "ScriptDefines.hpp"
#include "Memory/ScriptMemoryArena.hpp"
#include <memory>

namespace FLY_NAMESPACE
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
			if (!sInstance)
			{
				sInstance = new ScriptFoundation();
			}
			return *sInstance;
		}

		static void Destroy();
	private:
		ScriptFoundation();
		~ScriptFoundation();

	public:

		void Initialize();

		void ClearClasses();

		Class& CreateClass(const DataTypeID aTargetID, std::string_view aName);
		void DestroyClass(Class& aClass);
		const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& GetClasses();

		TypeManager& GetTypeManager();
		NodeExecutor& GetNodeExecutor();

	private:

		inline static ScriptFoundation* sInstance = nullptr;

		MemoryPool mMemoryPool;
		std::unique_ptr<TypeManager> mTypeManager;
		std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>> mClasses;

		std::unique_ptr<NodeExecutor> mNodeExecutor;

	};
}