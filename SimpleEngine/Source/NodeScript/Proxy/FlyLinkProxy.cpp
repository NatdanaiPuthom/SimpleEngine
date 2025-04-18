#include "FlyLinkProxy.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "FlyNodeGraphProxy.hpp"
#include "../Fly.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{
	using EntityID = size_t;

	constexpr size_t INVALID_ID = std::numeric_limits<size_t>::max();

	template<typename T>
	class ComponentPool
	{
	public:

		T* GetComponent(EntityID aEntityID)
		{
			const size_t index = myEntityIDToComponentIndex[aEntityID];
			if (index != INVALID_ID)
			{
				return &myComponents[index];
			}

			return nullptr;
		}

		T& AddComponent(EntityID aEntityID)
		{
			myEntityIDToComponentIndex[aEntityID] = myComponents.size();
			myDenseToEntity.emplace_back(aEntityID);
			return myComponents.emplace_back();
		}

		void RemoveComponent(EntityID aEntityID)
		{
			const size_t index = myEntityIDToComponentIndex[aEntityID];
			if (index == INVALID_ID)
			{
				return;
			}

			const EntityID backEntityID = myDenseToEntity.back();
			std::swap(myComponents[index], myComponents.back());
			myEntityIDToComponentIndex[backEntityID] = index;
			myEntityIDToComponentIndex[aEntityID] = INVALID_ID;
			myComponents.pop_back();
			myDenseToEntity.pop_back();
		}

		void Increment(const size_t aCount)
		{
			myEntityIDToComponentIndex.resize(myEntityIDToComponentIndex.size() + aCount, INVALID_ID);
		}

		std::vector<size_t> myEntityIDToComponentIndex;
		std::vector<T> myComponents;
		std::vector<EntityID> myDenseToEntity;
	};

	class ComponentPoolWrapper
	{
	public:

		ComponentPoolWrapper()
		{
		}

		template<typename T>
		void Init()
		{
			myComponentPoolPtr = new ComponentPool<T>();
			myGetComponentFunction = [](void* aComponentPoolPtr, EntityID aEntityID) -> void*
				{
					ComponentPool<T>& componentPool = *static_cast<ComponentPool<T>*>(aComponentPoolPtr);
					return componentPool.GetComponent(aEntityID);
				};

			myAddComponentFunction = [](void* aComponentPoolPtr, EntityID aEntityID) -> void*
				{
					ComponentPool<T>& componentPool = *static_cast<ComponentPool<T>*>(aComponentPoolPtr);
					return &componentPool.AddComponent(aEntityID);
				};

			myRemoveComponentFunction = [](void* aComponentPoolPtr, EntityID aEntityID) -> void
				{
					ComponentPool<T>& componentPool = *static_cast<ComponentPool<T>*>(aComponentPoolPtr);
					componentPool.RemoveComponent(aEntityID);
				};

			myIncrementFunction = [](void* aComponentPoolPtr, size_t aCount) -> void
				{
					ComponentPool<T>& componentPool = *static_cast<ComponentPool<T>*>(aComponentPoolPtr);
					componentPool.Increment(aCount);
				};
		}

		template<typename T>
		T* GetComponent(EntityID aEntityID)
		{
			return static_cast<T*>(myGetComponentFunction(myComponentPoolPtr, aEntityID));
		}

		template<typename T>
		T& AddComponent(EntityID aEntityID)
		{
			return *static_cast<T*>(myAddComponentFunction(myComponentPoolPtr, aEntityID));
		}

		template<typename T>
		void RemoveComponent(EntityID aEntityID)
		{
			myRemoveComponentFunction(myComponentPoolPtr, aEntityID);
		}

		void Increment()
		{
			myIncrementFunction(myComponentPoolPtr, 1);
		}
	private:

		void* myComponentPoolPtr = nullptr;
		void*(*myGetComponentFunction)(void* aComponentPool, EntityID aEntityID);
		void*(*myAddComponentFunction)(void*, EntityID aEntityID);
		void(*myRemoveComponentFunction)(void* aComponentPool, EntityID aEntityID);
		void(*myIncrementFunction)(void* aComponentPool, size_t aCount);

	};

	template<typename T>
	struct ComponentHash
	{
		inline static size_t ID = INVALID_ID;
	};

	class ECS
	{
	public:

		template<typename T>
		void RegisterComponent()
		{
			
			ComponentHash<T>::ID = myComponentPools.size();
			ComponentPoolWrapper c;
			c.Init<T>();
			myComponentPools.emplace_back(std::move(c));
		}

		template<typename T>
		T* GetComponent(EntityID aEntityID)
		{
			size_t componentHash = ComponentHash<T>::ID;

			return myComponentPools[componentHash].GetComponent<T>(aEntityID);
		}

		template<typename T>
		T& AddComponent(EntityID aEntityID)
		{
			size_t componentHash = ComponentHash<T>::ID;
			return myComponentPools[componentHash].AddComponent<T>(aEntityID);
		}

		template<typename T>
		void RemoveComponent(EntityID aEntityID)
		{
			size_t componentHash = ComponentHash<T>::ID;
			return myComponentPools[componentHash].RemoveComponent<T>(aEntityID);
		}

		EntityID CreateEntity()
		{
			EntityID entityID = myEntityCounter;
			myEntityCounter++;

			for (ComponentPoolWrapper& componentPoolWrapper : myComponentPools)
			{
				componentPoolWrapper.Increment();
			}

			return entityID;
		}

		std::vector<ComponentPoolWrapper> myComponentPools;
		EntityID myEntityCounter = 0;
	};
	struct TransformComponent
	{
		float f[3]{};
	};

	struct MoveComponent
	{
		float mySpeed = 3.f;
	};


	LinkProxy::LinkProxy(const LinkID aLinkID, const NodeGraphProxy& aNodeGraph)
		: mLinkID(aLinkID)
		, mNodeGraphVariant(aNodeGraph.GetVariant())
	{

		
	}

	LinkProxy::LinkProxy(const LinkID aLinkID, const NodeGraphVariantHandle& aNodeGraphVariant)
		: mLinkID(aLinkID)
		, mNodeGraphVariant(aNodeGraphVariant)
	{
		ECS ecs;
		ecs.RegisterComponent<TransformComponent>();
		ecs.RegisterComponent<MoveComponent>();

		EntityID entityID1 = ecs.CreateEntity();
		EntityID entityID2 = ecs.CreateEntity();
		EntityID entityID3 = ecs.CreateEntity();

		ecs.AddComponent<TransformComponent>(entityID1);
		ecs.AddComponent<MoveComponent>(entityID2);
		ecs.AddComponent<TransformComponent>(entityID3);
		ecs.AddComponent<MoveComponent>(entityID3);

		{
			[[maybe_unused]] auto* t0 = ecs.GetComponent<TransformComponent>(entityID1);
			[[maybe_unused]] auto* t1 = ecs.GetComponent<TransformComponent>(entityID2);
			[[maybe_unused]] auto* t2 = ecs.GetComponent<TransformComponent>(entityID3);
			[[maybe_unused]] auto* m0 = ecs.GetComponent<MoveComponent>(entityID1);
			[[maybe_unused]] auto* m1 = ecs.GetComponent<MoveComponent>(entityID2);
			[[maybe_unused]] auto* m2 = ecs.GetComponent<MoveComponent>(entityID3);
			std::cout << "hello" << std::endl;
		}

		ecs.RemoveComponent<TransformComponent>(entityID1);
		{

			[[maybe_unused]] auto* t1 = ecs.GetComponent<TransformComponent>(entityID1);
			[[maybe_unused]] auto* t2 = ecs.GetComponent<TransformComponent>(entityID2);
			[[maybe_unused]] auto* t3 = ecs.GetComponent<TransformComponent>(entityID3);
		}

	}

	bool LinkProxy::IsDestroyed() const
	{
		return GetLink().IsDestroyed();
	}

	PinProxy LinkProxy::GetInputPin() const
	{
		return PinProxy(GetLink().GetInputPinID(), NodeGraphProxy(mNodeGraphVariant));
	}

	PinProxy LinkProxy::GetOutputPin() const
	{
		return PinProxy(GetLink().GetOutputPinID(), NodeGraphProxy(mNodeGraphVariant));
	}

	LinkID LinkProxy::GetID() const
	{
		return mLinkID;
	}

	void LinkProxy::Destroy(CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLink(Internal::GetNodeGraph(mNodeGraphVariant), GetID(), aCommandTracker);
	}

	LinkProxy::operator bool() const
	{
		return mLinkID != InvalidID<LinkID>();
	}


	bool operator==(const LinkProxy& a, const LinkProxy& b)
	{
		return a.mLinkID == b.mLinkID && a.mNodeGraphVariant == b.mNodeGraphVariant;
	}

	const Link& LinkProxy::GetLink() const
	{
		return NodeGraphProxy(mNodeGraphVariant).GetNodeGraph().GetLink(mLinkID);
	}
}
