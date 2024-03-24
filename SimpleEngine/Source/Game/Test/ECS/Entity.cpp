#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Entity.hpp"

namespace ECS
{
	Entity::Entity()
	{
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
}