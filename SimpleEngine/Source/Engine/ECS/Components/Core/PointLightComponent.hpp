#pragma once
#include "Graphics/BufferData.hpp"

namespace ECS
{
	struct PointLightComponent final
	{
		PointLightComponent()
		{
			pointLightData.color.w = 10.0f;
			pointLightData.radius = 5.0f;
		};

		Graphics::PointLightData pointLightData;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(PointLightComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&PointLightComponent::pointLightData);
}