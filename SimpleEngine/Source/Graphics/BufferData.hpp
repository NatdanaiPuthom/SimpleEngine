#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"

struct alignas(16) FrameBufferData final
{
	Math::Matrix4x4f worldToClipMatrix;

	Math::Vector3f cameraPosition;
	const float paddingCameraPos = -1.0f;

	Math::Vector2ui resolution;
	const float paddingResolution[2] = { -1.0f };
};

struct alignas(16) ObjectBufferData final
{
	Math::Matrix4x4f modelWorldMatrix;
};

struct alignas(16) TimeBufferData final
{
	float totalTime = 0.0f;
	float deltaTime = 0.0f;
	const float padding[2] = { -1 };
};

struct alignas(16) LightBufferData final
{
	Math::Vector4f skyColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Math::Vector4f groundColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	Math::Vector3f directionalLightDirection;
	const float paddingDirectionalLightDirection = -1.0f;

	Math::Vector4f directionalLightColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	struct PointLightData
	{
		Math::Vector4f position;
		Math::Vector4f color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		float range = 0.0f;
		const float padding[3] = { -1.0f };
	} pointLights[8];

	unsigned int numberOfPointlights = 0;
	const float paddingPointlightData[3] = { -1.0f };
};

struct alignas(16) BonesBufferData
{
	Math::Matrix4x4f bonesTransform[SIMPLE_MAX_BONES];
};