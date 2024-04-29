#pragma once
#include "Game/Test/ECS/ECSInterface.hpp"
#include <vector>

namespace Simple
{
	class Entities final
	{
	public:
		Entities(std::vector<char**>& aAllEntities);
		~Entities();

		Entity operator[](const size_t aIndex);

		size_t GetSize() const;

	private:
		std::vector<char**>& myAllEntities;
	};
}