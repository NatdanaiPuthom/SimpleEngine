#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/Entities.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include <cassert>

namespace ECS
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

		if (size == 0)
		{
			static IEntity* entityPointer = nullptr;
			return std::ref(entityPointer);
		}

		if (aIndex < 0 || aIndex > size)
		{
			assert(false && "Index out of range");
			static IEntity* entityPointer = nullptr;
			return std::ref(entityPointer);
		}

		return reinterpret_cast<Entity>(*myAllEntities[aIndex]);
	}

	size_t Entities::GetSize() const
	{
		return myAllEntities.size();
	}
}