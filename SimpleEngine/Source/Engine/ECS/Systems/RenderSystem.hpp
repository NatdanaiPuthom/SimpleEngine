#pragma once
#include "Engine/ECS/Core/System.hpp"
#include <vector>

namespace ECS
{
	class RenderSystem : public ECS::System
	{
	public:
		RenderSystem(EntityManager* aECS);
		~RenderSystem() override;

		void Init() override;
		void Update() override;
		void Render() override;

		ECS::EntityID myEntityID;
	};
}