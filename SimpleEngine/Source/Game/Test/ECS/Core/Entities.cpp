#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/Entities.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	Entities::Entities(std::vector<char**>& aAllEntities)
		: myAllEntities(aAllEntities)
	{
	}

	Entities::~Entities()
	{
	}

	Entity Entities::operator[](const size_t aIndex)
	{
		const size_t size = GetSize();

		if (aIndex > size || size == 0)
		{
			static IEntity* entityPointer = nullptr;
			return std::ref(entityPointer);
		}

		return reinterpret_cast<IEntity*&>(*myAllEntities[aIndex]);
	}

	size_t Entities::GetSize() const
	{
		return myAllEntities.size();
	}
}