#pragma once
#include "Components/Base/Component.h"
#include <vector>
#include <string>

class GameObject final
{
public:
	std::string myName;
	const unsigned int myID;
	bool myCanPush;
	bool myCanStandOn;
	Math::Vector2i myPosition;
	bool myIsActive ;
	int myKeyCount;
public:
	GameObject(unsigned int aID)
		: myCanPush(false)
		, myCanStandOn(false)
		, myID(aID)
		, myName(std::string("GameObject") + std::to_string(aID))
		, myIsActive(true)
		, myKeyCount(0)
	{
	};

	~GameObject()
	{
		for (size_t i = 0; i < myComponents.size(); ++i)
		{
			delete myComponents[i];
		}
	};

	template<typename T>
	bool HasComponent() const
	{
		for (size_t i = 0; i < myComponents.size(); ++i)
		{
			const T* component = dynamic_cast<T*>(myComponents[i]);
			if (component)
				return true;
		}

		return false;
	}

	template<typename T>
	T* AddComponent()
	{
		static_assert(std::is_base_of_v<Component, T>);
		T* component = new T();
		AddComponent(component);
		return component;
	}

	void AddComponent(Component* aComponent)
	{
		myComponents.push_back(aComponent);
		myComponents.back()->myGameObject = this;
	}

	template<typename T>
	T* GetComponent()
	{
		for (size_t i = 0; i < myComponents.size(); ++i)
		{
			T* component = dynamic_cast<T*>(myComponents[i]);
			if (component)
				return component;
		}

		return nullptr;
	}


	template<typename T>
	const T* GetComponent() const
	{
		for (size_t i = 0; i < myComponents.size(); ++i)
		{
			T* component = dynamic_cast<T*>(myComponents[i]);
			if (component)
				return component;
		}

		return nullptr;
	}

	void Init(const GameInitContext& aContext)
	{
		for (size_t i = 0; i < myComponents.size(); ++i)
		{
			myComponents[i]->Init(aContext);
		}
	}

	void Update(const GameUpdateContext& aContext)
	{
		if (!myIsActive)
		{
			return;
		}
		for (size_t i = 0; i < myComponents.size(); ++i)
		{
			myComponents[i]->Update(aContext);
		}
	}

private:
	std::vector<Component*> myComponents;
};
