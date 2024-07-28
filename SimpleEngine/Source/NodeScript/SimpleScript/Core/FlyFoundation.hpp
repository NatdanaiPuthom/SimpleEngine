#pragma once
#include "FlyDefines.hpp"
#include "Memory/FlyMemoryArena.hpp"
#include <memory>

namespace FLY_NAMESPACE
{
	class NodeExecutor;
	class TypeManager;
	class Class;

	class Foundation final
	{
	public:

		inline static Foundation& GetInstance()
		{
			if (!sInstance)
			{
				sInstance = new Foundation();
			}
			return *sInstance;
		}

		static void Destroy();
	private:
		Foundation();
		~Foundation();

	public:

		void Initialize();

		void ClearClasses();

		Class& CreateClass(const DataTypeID aTargetID, std::string_view aName);
		void DestroyClass(Class& aClass);
		void SetClassName(std::string_view aOldName, std::string_view aNewName);
		const std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>>& GetClasses();

		TypeManager& GetTypeManager();
		NodeExecutor& GetNodeExecutor();

	private:

		inline static Foundation* sInstance = nullptr;

	public:

		MemoryPool mMemoryPool;
		std::unique_ptr<TypeManager> mTypeManager;
		std::unordered_map<DataTypeID, std::vector<std::unique_ptr<Class>>> mClasses;
		std::unordered_map<std::string_view, Class*> mClassesByName;

		std::unique_ptr<NodeExecutor> mNodeExecutor;

	};
}