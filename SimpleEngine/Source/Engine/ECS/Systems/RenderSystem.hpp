#pragma once
#include "Engine/ECS/Core/System.hpp"
#include <vector>

namespace ECS
{
	struct MeshComponent;
	struct TransformComponent;
	struct AnimationComponent;
}

namespace ECS
{
	class EntityComponentSystem;

	class RenderSystem : public ECS::System
	{
		struct StaticModelToRender
		{
			StaticModelToRender(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent)
				: transformComponent(aTransformComponent)
				, meshComponent(aMeshComponent)
			{
			}

			const ECS::TransformComponent* const transformComponent;
			const ECS::MeshComponent* const meshComponent;
		};

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
		explicit RenderSystem(EntityComponentSystem* aEntityComponentSystem);
		~RenderSystem() override;

		void Render() override;
		void LateRender() override;

		std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;
	private:
		std::vector<StaticModelToRender> myStaticModelToRender;
		std::vector<AnimatedModelToRender> myAnimatedModelToRender;
	};
}