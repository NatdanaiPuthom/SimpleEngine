#pragma once
#include "Graphics/Camera/Camera.hpp"

namespace ECS
{
	struct CameraComponent final
	{
		Graphics::Camera camera;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(CameraComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&CameraComponent::camera);
}