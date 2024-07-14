#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class RenderLightSystem final : public ECS::System
	{
	public:
		explicit RenderLightSystem(EntityComponentSystem* aEntityComponentSystem);
		~RenderLightSystem();

		void Init() override;
		void Update() override;
		void Render() override;
		void RenderSkyBoxAndDirectionalLight() const;

		std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;

	private:
		bool FindAndSetSkyBox();
		bool FindAndSetDirectionalLight();
	private:
		EntityID myEntityWithSkyBoxID;
		EntityID myEntityWithDirectionalLightID;
	};
}