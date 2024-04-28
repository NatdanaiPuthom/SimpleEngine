#pragma once
#include <vector>
#include <array>

namespace Simple
{
	class Entity;
}

namespace Simple
{
	class Entities final
	{
	public:
		Entities(std::vector<char**>& aAllEntities);
		~Entities();

		Entity*& operator[](const size_t aIndex);

		size_t GetSize() const;

	private:
		std::vector<char**>& myAllEntities;
		std::array<int*, 5> value;
	};
}