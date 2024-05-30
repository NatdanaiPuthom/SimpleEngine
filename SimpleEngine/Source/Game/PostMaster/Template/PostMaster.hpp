#pragma once
#include <any>
#include <unordered_map>
#include <unordered_set>

#include "Game/PostMaster/EventTypes.hpp"

namespace Simple
{
	template<typename MessageType>
	struct Message
	{
		MessageType myType;
		std::any myData;
	};

	struct MessageEmil
	{
		std::any data;
	};

	template<typename MessageType>
	class Observer
	{
		template<typename MessageType>
		friend class PostMaster;
	public:
		virtual ~Observer() = default;
	private:
		virtual void ReceiveMessage(const Simple::Message<MessageType>& aMessage) = 0;
	};
}

namespace Simple
{
	template<typename MessageType>
	class PostMaster
	{
	public:
		PostMaster();
		~PostMaster();

		void AddObserver(Simple::Observer<MessageType>* aObserver, const MessageType aMessageType);
		void RemoveObserver(Simple::Observer<MessageType>* aObserver, const MessageType aMessageType);
		void NotifyObservers(const Simple::Message<MessageType>& aMessage);
	private:
		std::unordered_map<MessageType, std::unordered_set<Simple::Observer<MessageType>*>> myObservers;
	};

	template<typename MessageType>
	inline PostMaster<MessageType>::PostMaster()
	{
	}

	template<typename MessageType>
	inline PostMaster<MessageType>::~PostMaster()
	{
	}

	template<typename MessageType>
	inline void PostMaster<MessageType>::AddObserver(Simple::Observer<MessageType>* aObserver, const MessageType aMessageType)
	{
		myObservers[aMessageType].insert(aObserver);
	}

	template<typename MessageType>
	inline void PostMaster<MessageType>::RemoveObserver(Simple::Observer<MessageType>* aObserver, const MessageType aMessageType)
	{
		myObservers[aMessageType].erase(aObserver);
	}

	template<typename MessageType>
	inline void PostMaster<MessageType>::NotifyObservers(const Simple::Message<MessageType>& aMessage)
	{
		for (Simple::Observer<MessageType>* observers : myObservers.at(aMessage.myType))
		{
			observers->ReceiveMessage(aMessage);
		}
	}

	class PostMasterEmil
	{


	public:

		PostMasterEmil() = default;
		~PostMasterEmil() = default;

		using SubscribeFunction = std::function<void(const void*, size_t, void*)>;

		template<typename ClassType, typename DataType>
		using MemberSignature = void(ClassType::*)(const DataType&);

		struct FuncPair
		{
			void* owner;
			//SubscribeFunction function;
			std::function<void(const void*, size_t, void*)> func;
			size_t dataTypeID;

			bool operator==(const FuncPair& aOther) const
			{
				return owner == aOther.owner;
			}
		};

		struct FuncPairHasher
		{
			size_t operator()(const FuncPair& aPair) const
			{
				return reinterpret_cast<size_t>(aPair.owner);
			}
		};

		template<typename EventType, typename ClassType, typename DataType>
		void AddObserver(EventType aEventType, ClassType* anObject, MemberSignature<ClassType, DataType> aFunction);

		template<typename EventType, typename ClassType>
		void RemoveObserver(const EventType aEventType, ClassType* anObject);

		template<typename EventType, typename DataType>
		void NotifyObservers(EventType aEventType, const DataType& aData = = DataType{});

	private:

		std::unordered_map<size_t, std::unordered_map<size_t, std::unordered_set<FuncPair, FuncPairHasher>>> myObservers;

	};

	template< typename EventType, typename ClassType, typename DataType>
	inline void PostMasterEmil::AddObserver(EventType aEventType, ClassType* anObject, MemberSignature<ClassType, DataType> aFunction)
	{
		size_t typeID = typeid(EventType).hash_code();
		size_t eventType = static_cast<size_t>(aEventType);
		SubscribeFunction func = [aFunction](const void* aData, size_t aDataTypeID, void* anObject) -> void
			{
				assert(aDataTypeID == typeid(DataType).hash_code());
				const DataType& data = *reinterpret_cast<const DataType*>(aData);
				ClassType* object = reinterpret_cast<ClassType*>(anObject);
				(object->*aFunction)(data);
			};
		FuncPair funcPair{ anObject, func, typeid(DataType).hash_code() };
		myObservers[typeID][eventType].insert(funcPair);
	}


	template<typename EventType, typename ClassType>
	inline void PostMasterEmil::RemoveObserver(const EventType aEventType, ClassType* anObject)
	{
		size_t typeID = typeid(EventType).hash_code();
		size_t eventType = static_cast<size_t>(aEventType);
		myObservers[typeID][eventType].erase(FuncPair{ anObject });
	}

	template<typename EventType, typename DataType>
	inline void PostMasterEmil::NotifyObservers(EventType aEventType, const DataType& aData)
	{
		size_t typeID = typeid(EventType).hash_code();
		size_t eventType = static_cast<size_t>(aEventType);
		const auto& observers = myObservers[typeID][eventType];
		for (const auto& observer : observers)
		{
			observer.func(&aData, typeid(DataType).hash_code(), observer.owner);
		}
	}
}