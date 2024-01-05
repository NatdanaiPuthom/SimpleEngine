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
}