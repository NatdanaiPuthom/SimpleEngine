#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class RenderLightSystem final : public ECS::System
	{
	public:
		RenderLightSystem(EntityComponentSystem* aEntityComponentSystem);
		~RenderLightSystem();

		void Init() override;
		void Update() override;
		void Render() override;
		void RenderSkyBoxAndDirectionalLight() const;

		virtual std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const
		{
			return std::make_unique<RenderLightSystem>(aEntityComponentSystem);
		}

	private:
		bool FindAndSetSkyBox();
		bool FindAndSetDirectionalLight();
	private:
		EntityID myEntityWithSkyBoxID;
		EntityID myEntityWithDirectionalLightID;
	};
}