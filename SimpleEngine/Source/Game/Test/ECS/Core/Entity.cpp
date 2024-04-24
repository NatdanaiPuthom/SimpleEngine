#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	Entity::Entity(const size_t aID)
	{
		myID = aID;
		myName = "Entity " + std::to_string(myID);
	}

	void Entity::SetName(const std::string& aName)
	{
		myName = aName;
	}

	std::vector<size_t> Entity::GetAllComponentsID() const
	{
		std::vector<size_t> componentIDs;
		componentIDs.reserve(myComponents.size());

		for (const auto& [key, value] : myComponents)
		{
			componentIDs.push_back(value);
		}

		return componentIDs;
	}

	std::vector<std::string> Entity::GetAllComponentsName() const
	{
		std::vector<std::string> componentNames;
		componentNames.reserve(myComponents.size());

		for (const auto& [key, value] : myComponents)
		{
			std::string fullname = key.name();
			size_t lastSpacePos = fullname.find_last_of("::");

			if (lastSpacePos != std::string::npos && lastSpacePos + 1 < fullname.length())
			{
				componentNames.push_back(fullname.substr(lastSpacePos + 1));
			}
			else
			{
				componentNames.push_back(fullname);
			}
		}

		return componentNames;
	}

	const std::string& Entity::GetName() const
	{
		return myName;
	}

	size_t Entity::GetID() const
	{
		return myID;
	}
}