#pragma once
#include "Engine/ECS/Core/System.hpp"
#include <unordered_map>
#include <memory>

namespace ECS
{
	class ECSGameSystem final
	{

	public:
		template<typename T>
		static void AddSystem()
		{
			const size_t hashCode = typeid(T).hash_code();

			if (mySystems.contains(hashCode))
			{
				assert(false && "System already exist.");
				return;
			}

			mySystems.emplace(hashCode, std::make_unique<T>(nullptr));
		}

		static std::unordered_map<size_t, const std::unique_ptr<ECS::System>> mySystems;
	};
}