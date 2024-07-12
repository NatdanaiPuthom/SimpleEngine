#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class EntityComponentSystem;

	class RenderSystem : public ECS::System
	{
	public:
		explicit RenderSystem(EntityComponentSystem* aEntityComponentSystem);
		~RenderSystem() override;

		void Init() override;
		void Update() override;
		void Render() override;

		std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const;
	};
}