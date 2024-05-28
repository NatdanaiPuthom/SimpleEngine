#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class SkyBoxAndDirectionalLightSystem final : public ECS::System
	{
	public:
		SkyBoxAndDirectionalLightSystem(EntityManager* aECS);
		~SkyBoxAndDirectionalLightSystem();

		void Init() override;
		void Update() override;
		void Render() override;

	private:
		EntityID mySkyBoxID;
		EntityID myDirectionalLightID;
	};
}