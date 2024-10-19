#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <any>

class TypeErasedPostMaster final
{
	template<typename ClassType, typename DataType>
	using MemberSignature = void(ClassType::*)(const DataType&);
public:



	template<typename EventType, typename ClassType, typename DataType>
	void Subscribe(const EventType aEventType, ClassType* const aObject, MemberSignature<ClassType, DataType> aFunction)
	{
		const EventTypeID typeID = typeid(EventType).hash_code();
		const EventTypeID id = static_cast<EventTypeID>(aEventType);

		Subscriber subscriber
		{
			.owner = aObject,
	#ifdef _DEBUG
			.function = [aFunction, aObject](const void* aData, const size_t aHash) -> void
			{
				assert(aHash == typeid(DataType).hash_code());
				(aObject->*aFunction)(*reinterpret_cast<const DataType*>(aData));
			}
	#else
			.function = [aFunction, aObject](const void* aData) -> void { (aObject->*aFunction)(*reinterpret_cast<const DataType*>(aData)); }
	#endif
		};
		mySubscribers[typeID][id].emplace(subscriber);
	}

	template<typename EventType, typename DataType>
	void Subscribe(const EventType aEventType, void(*aFunction)(const DataType&))
	{
		const EventTypeID typeID = typeid(EventType).hash_code();
		const EventTypeID id = static_cast<EventTypeID>(aEventType);

		Subscriber subscriber
		{
			.owner = aFunction,
	#ifdef _DEBUG
			.function = [aFunction](const void* aData, const size_t aHash) -> void
			{
				assert(aHash == typeid(DataType).hash_code());
				(aFunction)(*reinterpret_cast<const DataType*>(aData));
			}
	#else
			.function = [aFunction, aObject](const void* aData) -> void { (aObject->*aFunction)(*reinterpret_cast<const DataType*>(aData)); }
	#endif
		};
		mySubscribers[typeID][id].emplace(subscriber);
	}

	template<typename EventType, typename ClassType>
	void Unsubscribe(const EventType aEventType, ClassType* const aObject)
	{
		auto it = mySubscribers.find(typeid(EventType).hash_code());
		if (it == mySubscribers.end())
		{
			return;
		}

		auto& subscribersByEventType = it->second;
		auto it2 = subscribersByEventType.find(static_cast<EventTypeID>(aEventType));
		if (it2 == subscribersByEventType.end())
		{
			return;
		}


		auto& subscribers = it2->second;
		subscribers.erase(Subscriber{ .owner = aObject });
	}

	template<typename EventType, typename DataType>
	void NotifySubscribers(const EventType aEventType, const DataType& aData) const
	{
		auto it = mySubscribers.find(typeid(EventType).hash_code());
		if (it == mySubscribers.end())
		{
			return;
		}

		auto& subscribersByEventType = it->second;
		auto it2 = subscribersByEventType.find(static_cast<EventTypeID>(aEventType));
		if (it2 == subscribersByEventType.end())
		{
			return;
		}

		const auto& subscribers = it2->second;
		for (const auto& subscriber : subscribers)
		{
#ifdef _DEBUG
			subscriber.function(&aData, typeid(DataType).hash_code());
#else
			subscriber.function(&aData);
#endif 
		}
	}

	template<typename EventType>
	void NotifySubscribers(const EventType aEventType) const
	{
		NotifySubscribers(aEventType, std::nullptr_t{});
	}

private:

#ifdef _DEBUG
	using SubscribeFunction = std::function<void(const void* aData, size_t aDataHash)>;
#else
	using SubscribeFunction = std::function<void(const void* aData)>;
#endif
	struct Subscriber final
	{
		void* owner;
		SubscribeFunction function;


		friend bool operator==(const Subscriber& a, const Subscriber& b)
		{
			return a.owner == b.owner;
		}
	};

	struct SubscriberHasher final
	{
		size_t operator()(const Subscriber& aSubscriber) const
		{
			return reinterpret_cast<size_t>(aSubscriber.owner);
		}

	};

	using EventTypeID = size_t;

	std::unordered_map<EventTypeID, std::unordered_map<EventTypeID, std::unordered_set<Subscriber, SubscriberHasher>>> mySubscribers;

};

class BufferedTypeErasedPostMaster final
{

#ifdef _DEBUG
	using SubscribeFunction = std::function<void(const void* aData, size_t aDataHash)>;
#else
	using SubscribeFunction = std::function<void(const void* aData)>;
#endif
	struct Subscriber final
	{
		void* owner;
		SubscribeFunction function;


		friend bool operator==(const Subscriber& a, const Subscriber& b)
		{
			return a.owner == b.owner;
		}
	};

	struct SubscriberHasher final
	{
		size_t operator()(const Subscriber& aSubscriber) const
		{
			return reinterpret_cast<size_t>(aSubscriber.owner);
		}

	};

	using EventTypeID = size_t;


	struct Event final
	{

		template<typename EventType, typename DataType>
		Event(EventType aEventID, const DataType& aData)
			: myEventType(typeid(EventType).hash_code())
			, myEventID(static_cast<size_t>(aEventID))
			, myDataTypeID(typeid(DataType).hash_code())
			, myData(aData)
			, myDataConverter([](const std::any& aData) -> const void* { return &std::any_cast<const DataType&>(aData); })
		{

		}

		const void* GetData() const
		{
			return myDataConverter(myData);
		}

		size_t myEventType;
		size_t myEventID;
		size_t myDataTypeID;
		std::any myData;
		const void* (*myDataConverter)(const std::any& aData);
	};

	template<typename ClassType, typename DataType>
	using MemberSignature = void(ClassType::*)(const DataType&);
public:



	template<typename EventType, typename ClassType, typename DataType>
	void Subscribe(const EventType aEventType, ClassType* const aObject, MemberSignature<ClassType, DataType> aFunction)
	{
		const EventTypeID typeID = typeid(EventType).hash_code();
		const EventTypeID id = static_cast<EventTypeID>(aEventType);

		Subscriber subscriber
		{
			.owner = aObject,
	#ifdef _DEBUG
			.function = [aFunction, aObject](const void* aData, const size_t aHash) -> void
			{
				assert(aHash == typeid(DataType).hash_code());
				(aObject->*aFunction)(*reinterpret_cast<const DataType*>(aData));
			}
	#else
			.function = [aFunction, aObject](const void* aData) -> void { (aObject->*aFunction)(*reinterpret_cast<const DataType*>(aData)); }
	#endif
		};
		mySubscribers[typeID][id].emplace(subscriber);
	}

	template<typename EventType, typename ClassType>
	void Unsubscribe(const EventType aEventType, ClassType* const aObject)
	{
		auto it = mySubscribers.find(typeid(EventType).hash_code());
		if (it == mySubscribers.end())
		{
			return;
		}

		auto& subscribersByEventType = it->second;
		auto it2 = subscribersByEventType.find(static_cast<EventTypeID>(aEventType));
		if (it2 == subscribersByEventType.end())
		{
			return;
		}


		auto& subscribers = it2->second;
		subscribers.erase(Subscriber{ .owner = aObject });
	}

	template<typename EventType, typename DataType>
	void RegisterEvent(const EventType aEventType, const DataType& aData)
	{
		myEvents.push_back(Event(aEventType, aData));
	}

	void NotifyAllSubscribers()
	{
		for (const Event& e : myEvents)
		{
			NotifySubscribers(e);
		}

		myEvents.clear();
	}

private:

	void NotifySubscribers(const Event& aEvent)
	{
		auto it = mySubscribers.find(aEvent.myEventType);
		if (it == mySubscribers.end())
		{
			return;
		}

		auto& subscribersByEventType = it->second;
		auto it2 = subscribersByEventType.find(aEvent.myEventID);
		if (it2 == subscribersByEventType.end())
		{
			return;
		}

		const auto& subscribers = it2->second;
		for (const auto& subscriber : subscribers)
		{
#ifdef _DEBUG
			subscriber.function(aEvent.GetData(), aEvent.myDataTypeID);
#else
			subscriber.function(aEvent.GetData());
#endif 
		}
	}


private:


	std::unordered_map<EventTypeID, std::unordered_map<EventTypeID, std::unordered_set<Subscriber, SubscriberHasher>>> mySubscribers;
	std::vector<Event> myEvents;
};