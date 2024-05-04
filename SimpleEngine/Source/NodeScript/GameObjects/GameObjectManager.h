#pragma once
#include "GameObjects/Base/GameObject.h"

class GameObjectManager
{
public:
	GameObjectManager() : myCurrentID(0)
	{
	}

	~GameObjectManager()
	{
		for (size_t i = 0; i < myGameObjects.size(); ++i)
		{
			delete myGameObjects[i];
			myGameObjects[i] = nullptr;
		}
	}

	unsigned int CreateGameObject()
	{
		if (myAvaliableIDs.empty())
		{
			GameObject* gameObject = new GameObject(myCurrentID);
			myGameObjects.push_back(gameObject);
			myCurrentID++;

			return gameObject->myID;
		}
		else
		{
			const unsigned int ID = myAvaliableIDs.back();
			GameObject* gameObject = new GameObject(ID);

			myGameObjects[ID] = gameObject;
			myAvaliableIDs.pop_back();

			return gameObject->myID;
		}
	}

	void RemoveGameObject(const unsigned int aID)
	{
		//myAvaliableIDs.push_back(aID);
		for (GameObject* gameObject : myGameObjects)
		{
			if (gameObject->myID == aID)
			{
				gameObject->myIsActive = false;
			}
		}

		//IEntity*&
		//Entity IEntity*
		//Entity->
		//delete myGameObjects[aID];
		//myGameObjects.erase(myGameObjects.begin() + aID);
	}

	void DeleteGameObjectsToBeDeleted()
	{
		for (size_t j = 0; j < myAvaliableIDs.size(); ++j)
		{
			for (size_t i = 0; i < myGameObjects.size(); ++i)
		
			{
				if (myAvaliableIDs[j] == myGameObjects[i]->myID)
				{
		
					
					myGameObjects[i] = nullptr;
					
					delete myGameObjects[i];
					myGameObjects.erase(myGameObjects.begin() + i);
				}
			}
		}
		myAvaliableIDs.clear();
	}

	void RemoveGameObjectFromList(const unsigned int aID)
	{

		for (size_t i = 0; i < myGameObjects.size(); ++i)
		{
			if (myGameObjects[i]->myID == aID)
			{
				myAvaliableIDs.push_back(aID);
				delete myGameObjects[aID];
				myGameObjects[aID] = nullptr;
				myGameObjects.erase(myGameObjects.begin() + i);
				break;
			}
		}
	}

	GameObject* GetGameObject(const unsigned int aID)
	{

		for (size_t i = 0; i < myGameObjects.size(); ++i)
		{
			if (myGameObjects[i]->myID == aID)
			{
				return myGameObjects[i];
			}
		}

		return nullptr;
	}

	std::vector<GameObject*> GetAllGameObjects()
	{
		std::vector<GameObject*> noNullPointerGameObject;
		for (size_t i = 0; i < myGameObjects.size(); ++i)
		{
			if (myGameObjects[i])
			{
				noNullPointerGameObject.push_back(myGameObjects[i]);
			}
		}

		return noNullPointerGameObject;
	}

	template<typename T>
	std::vector<GameObject*> GetGameObjectsWithComponent()
	{
		std::vector<GameObject*> gameObjects;
		gameObjects.reserve(myGameObjects.size());

		for (const auto& gameObject : myGameObjects)
		{
			if (!gameObject)
				continue;
			if (gameObject->HasComponent<T>())
				gameObjects.push_back(gameObject);
		}

		return gameObjects;
	}

private:
	std::vector<GameObject*> myGameObjects;
	std::vector<unsigned int> myGameObjectsToDelete;
	std::vector<unsigned int> myAvaliableIDs;
	unsigned int myCurrentID;
};