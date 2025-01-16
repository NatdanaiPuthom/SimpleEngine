#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Graphics/Camera/Camera.hpp"

namespace ECS
{
	COMPONENT(CameraComponent) final
	{
		Graphics::Camera camera;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&CameraComponent::camera);
}