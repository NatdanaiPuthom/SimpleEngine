#pragma once
#include "Engine/ECS/Core/System.hpp"
#include <vector>
#include <memory>

namespace ECS
{
	struct MeshComponent;
	struct TransformComponent;
	struct AnimationComponent;
}

namespace Graphics
{
	struct MeshInstance;
}

namespace ECS
{
	class RenderSystem : public ECS::System
	{
		struct AnimatedModelToRender
		{
			AnimatedModelToRender(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent, const ECS::AnimationComponent* aAnimationComponent)
				: transformComponent(aTransformComponent)
				, meshComponent(aMeshComponent)
				, animationComponent(aAnimationComponent)
			{
			}

			const ECS::TransformComponent* const transformComponent;
			const ECS::MeshComponent* const meshComponent;
			const ECS::AnimationComponent* const animationComponent ;
		};

	public:
		RenderSystem();
		~RenderSystem() override;

		void Render(EntityComponentSystem* aEntityComponentSystem) override;
		void LateRender(EntityComponentSystem* aEntityComponentSystem) override;

		std::unique_ptr<System> Clone() const override;
	private:
		void RenderUnlitModels(EntityComponentSystem* aEntityComponentSystem);
	private:
		std::vector<Graphics::MeshInstance> myStaticModelToRender;
		std::vector<AnimatedModelToRender> myAnimatedModelToRender;
	};
}