#pragma once
#include "FlyDefines.hpp"
#include "Memory/FlyMemoryArena.hpp"
#include "Variable/FlyVariableRef.hpp"
#include "Node/FlyNodeRef.hpp"
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

		Class& GetClassByID(ClassID aID);
		const std::vector< std::unique_ptr<Class>>& GetClasses() const;

		TypeManager& GetTypeManager();
		NodeExecutor& GetNodeExecutor();

		const VariableRef& GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef) const;

		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef) const;

	private:

		inline static Foundation* sInstance = nullptr;

	public:

		MemoryPool mMemoryPool;
		std::unique_ptr<TypeManager> mTypeManager;
		std::vector<std::unique_ptr<Class>> mClasses;

		std::unique_ptr<NodeExecutor> mNodeExecutor;

		std::unordered_map<GlobalNodeRef, VariableRef, GlobalNodeRefHasher> mNodeRefToVarRef;

	};
}