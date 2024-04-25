#pragma once
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace Simple
{
	class EntityE;

	class EntityM final
	{
	public:
		EntityM();
		~EntityM();

		EntityE& CreateEntity();
		
	private:
		std::vector<EntityE> myEntities;
	};

	class EntityE final
	{
	public:
		EntityE(int b);
		~EntityE() = default;

		int a;
	};
}