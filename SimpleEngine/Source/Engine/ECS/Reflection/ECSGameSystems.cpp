#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSGameSystems.hpp"

namespace ECS
{
	std::unordered_map<size_t, const std::unique_ptr< ECS::System>> ECSGameSystem::mySystems{};
}