#pragma once
#include "Engine/ECS/ECSInterface.hpp"
#include <vector>

namespace ECS
{
	class Entities final
	{
	public:
		Entities(std::vector<char**>& aAllEntities);
		~Entities();

		Entity operator[](const size_t aIndex);
		const Entity operator[](const size_t aIndex) const;

		size_t GetEntityCount() const;

	private:
		std::vector<char**>& myAllEntities;
	};
}