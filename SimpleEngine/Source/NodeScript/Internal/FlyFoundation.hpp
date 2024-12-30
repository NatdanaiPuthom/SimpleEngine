#pragma once
#include "../FlyDefines.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include "../DataType/FlyVariableRef.hpp"
#include "../Node/FlyNodeRef.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../Function/FlyTraitManager.hpp"
#include "../Memory/FlyHeapObject.hpp"
#include <memory>
#include <string_view>

namespace FLY_NAMESPACE
{
	class NodeExecutor;
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

		NodeExecutor& GetNodeExecutor();

		const VariableRef& GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef) const;

		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef) const;

	private:

		inline static Foundation* sInstance = nullptr;

	public:

		MemoryPool mMemoryPool;
		NodeTypeManager mNodeTypeManager;
		PinTypeManager mPinTypeManager;
		DataTypeManager mDataTypeManager;
		TraitManager mTraitManager;

		HeapObject<NodeExecutor> mNodeExecutor;
		EventGraph mNodeGraphCopy;

		std::unordered_map<GlobalNodeRef, VariableRef, GlobalNodeRefHasher> mNodeRefToVarRef;

	};
}