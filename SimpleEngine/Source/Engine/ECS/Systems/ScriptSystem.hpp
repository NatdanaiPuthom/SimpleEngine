#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class ScriptSystem final : public System
	{
	public:

		ScriptSystem();
		void Init(EntityComponentSystem* aEntityComponentSystem) override;
		void Update(EntityComponentSystem* aEntityComponentSystem) override;

		std::unique_ptr<System> Clone() const override;
	};
}