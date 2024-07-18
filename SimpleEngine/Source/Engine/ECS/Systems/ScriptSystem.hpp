#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{

	class ScriptSystem final : public System
	{
	public:

		ScriptSystem(EntityComponentSystem* aECS);


		void Update() override;

		std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;

	private:


	};
}