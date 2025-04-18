#pragma once
#include "../FlyDefines.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include "../DataType/FlyVariableRef.hpp"
#include "../Node/FlyNodeRef.hpp"
#include "../Graph/FlyEventGraph.hpp"
#include "../DataType/FlyDataTypeManager.hpp"
#include "../Node/FlyNodeTypeManager.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../Trait/FlyTraitManager.hpp"
#include "../Memory/FlyHeapObject.hpp"
#include <memory>
#include <string_view>

namespace FLY_NAMESPACE
{
	class NodeExecutor;

	class Foundation final
	{
		struct FoundationProxy
		{
			FoundationProxy(Foundation*& aStaticFoundation)
			{
				aStaticFoundation = new Foundation();
			}
		};

	public:

		inline static Foundation& GetInstance()
		{
			static FoundationProxy p(sInstance);
			return *sInstance;
		}

		static void Destroy();

	private:

		Foundation();
		~Foundation();

	public:

		void Initialize();

		MemoryPool& GetMemoryPool();
		const MemoryPool& GetMemoryPool() const;
		NodeTypeManager& GetNodeTypeManager();
		const NodeTypeManager& GetNodeTypeManager() const;
		PinTypeManager& GetPinTypeManager();
		const PinTypeManager& GetPinTypeManager() const;
		DataTypeManager& GetDataTypeManager();
		const DataTypeManager& GetDataTypeManager() const;
		TraitManager& GetTraitManager();
		const TraitManager& GetTraitManager() const;
		NodeExecutor& GetNodeExecutor();
		const NodeExecutor& GetNodeExecutor() const;
		EventGraph& GetNodeGraphCopy();
		const EventGraph& GetNodeGraphCopy() const;
		const VariableRef& GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef) const;
		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef) const;

	private:

		inline static Foundation* sInstance = nullptr;

	private:

		MemoryPool mMemoryPool;
		NodeTypeManager mNodeTypeManager;
		PinTypeManager mPinTypeManager;
		DataTypeManager mDataTypeManager;
		TraitManager mTraitManager;

		HeapObject<NodeExecutor> mNodeExecutor;
		EventGraph mNodeGraphCopy;

	public:

		std::unordered_map<GlobalNodeRef, VariableRef, GlobalNodeRefHasher> mNodeRefToVarRef;
	};
}