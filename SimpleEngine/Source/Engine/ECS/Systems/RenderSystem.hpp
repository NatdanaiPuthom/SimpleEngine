#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class EntityComponentSystem;

	class RenderSystem : public ECS::System
	{
	public:
		RenderSystem(EntityComponentSystem* aEntityComponentSystem);
		~RenderSystem() override;

		void Init() override;
		void Update() override;
		void Render() override;
	};
}