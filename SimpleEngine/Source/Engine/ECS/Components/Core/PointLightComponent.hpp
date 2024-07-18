#pragma once
#include "Graphics/BufferData.hpp"

namespace ECS
{
	struct PointLightComponent final
	{
		Graphics::PointLightData pointLightData;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(PointLightComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&PointLightComponent::pointLightData);
}