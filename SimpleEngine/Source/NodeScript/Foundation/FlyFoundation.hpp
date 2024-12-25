#pragma once
#include "FlyDefines.hpp"
#include "Memory/FlyMemoryArena.hpp"
#include "Variable/FlyVariableRef.hpp"
#include "Node/FlyNodeRef.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "../Type/FlyTypeManager.hpp"
#include <memory>
#include <string_view>
#include "../Memory/FlyHeapObject.hpp"

namespace FLY_NAMESPACE
{
	class NodeExecutor;
	class TypeManager;
	class Class;
	class Struct;

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

		StructID CreateStruct(std::string_view aName);
		ClassID CreateClass(const DataTypeID aTargetID, std::string_view aName);
		void DestroyClass(Class& aClass);

		Struct& GetStruct(StructID aID);
		const std::vector<HeapObject<Class>>& GetClasses() const;

		TypeManager& GetTypeManager();
		NodeExecutor& GetNodeExecutor();

		const VariableRef& GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef) const;

		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef) const;

	private:

		inline static Foundation* sInstance = nullptr;

	public:

		MemoryPool mMemoryPool;
		TypeManager mTypeManager;
		std::vector<HeapObject<Class>> mClasses;
		std::vector<HeapObject<Struct>> mStructs;

		HeapObject<NodeExecutor> mNodeExecutor;
		EventGraph mNodeGraphCopy;

		std::unordered_map<GlobalNodeRef, VariableRef, GlobalNodeRefHasher> mNodeRefToVarRef;

	};
}