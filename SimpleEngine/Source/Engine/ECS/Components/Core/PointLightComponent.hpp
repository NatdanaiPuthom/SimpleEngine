#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Graphics/BufferData.hpp"

namespace ECS
{
	COMPONENT(PointLightComponent) final
	{
		PointLightComponent()
		{
			pointLightData.color.w = 10.0f;
			pointLightData.radius = 5.0f;
		};

		Graphics::PointLightData pointLightData;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&PointLightComponent::pointLightData);
}