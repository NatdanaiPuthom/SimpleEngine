#include "EntityComponentSystem/ComponentManager.hpp"
#include "EntityComponentSystem/EntityID.hpp"
#include <iostream>
#include <chrono>
#include <string>

namespace Simple
{
	struct Identifier
	{
		std::string name;
		std::string tag;

		Identifier(const std::string& aName = "Entity", const std::string& aTag = "Default") : name(aName), tag(aTag) {}
	};

	struct Transform
	{
		float x;
		float y;
		float z;

		Transform(const float aX = 0.0f, const float aY = 0.0f, const float aZ = 0.0f) : x(aX), y(aY), z(aZ) {}
	};

	class ECS
	{
		using Index = size_t;
	public:
		ECS() :myNextID(EntityID(0)) {}

		EntityID CreateEntity()
		{
			Identifier identifer;
			identifer.name = "Entity " + std::to_string(myNextID);
			identifer.tag = "Default";

			return CreateEntity(identifer);;
		}

		EntityID CreateEntity(const Identifier& aIdentifie)
		{
			EntityID newID = myNextID++;

			myEntityIDMap[newID] = myAllEntityIDs.size();
			myAllEntityIDs.push_back(newID);

			AddComponent<Identifier>(newID, aIdentifie);
			AddComponent<Transform>(newID);

			return newID;
		}

		bool RemoveEntity(const EntityID& aEntityID)
		{
			auto it = myEntityIDMap.find(aEntityID);

			if (it != myEntityIDMap.end())
			{
				myAllEntityIDs[it->second] = myAllEntityIDs.back();
				myAllEntityIDs.pop_back();
				myEntityIDMap.erase(it);
			}

			return myComponentManager.RemoveAllComponents(aEntityID);
		}

		template<typename T>
		T* AddComponent(const EntityID aEntityID, const T& aComponent = T())
		{
			auto it = myEntityIDMap.find(aEntityID);

			if (it == myEntityIDMap.end())
			{
				return nullptr;
			}

			return myComponentManager.AddComponent<T>(aEntityID, aComponent);
		}

		template<typename T>
		T* GetComponent(const EntityID aEntityID)
		{
			return myComponentManager.GetComponent<T>(aEntityID);
		}

		const std::vector<EntityID>& GetAllEntityIDs()
		{
			return myAllEntityIDs;
		}

	private:
		ComponentManager myComponentManager;
		std::unordered_map<EntityID, Index, EntityID::Hash> myEntityIDMap;
		std::vector<EntityID> myAllEntityIDs;
		EntityID myNextID;
	};
}

//static void ResetPosition(Position& aPosition, float x, float y, float z)
//{
//	aPosition.x = x;
//	aPosition.y = y;
//	aPosition.z = z;
//}

using namespace Simple;

int main()
{
	ECS ecs;
	EntityID entityID1 = ecs.CreateEntity();

	auto a = ecs.GetComponent<Identifier>(entityID1);

	if (a != nullptr)
	{
		std::cout << a->name << std::endl;
	}

	/*Simple::ComponentManager componentManager;

	constexpr size_t size = 10;

	std::chrono::high_resolution_clock::time_point myStartTimerTime = std::chrono::high_resolution_clock::now();

	for (size_t i = 0; i < size; ++i)
	{
		componentManager.AddComponent<Position>(Simple::EntityID(i));
	}

	double time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTimerTime).count();

	std::cout << "Time took to Add: " << time << std::endl;

	myStartTimerTime = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < size; ++i)
	{
		componentManager.RemoveComponent<Position>(Simple::EntityID(i));
	}
	time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTimerTime).count();

	std::cout << "Time took to Remove: " << time << std::endl;*/

	return 0;
}