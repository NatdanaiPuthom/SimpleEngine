#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"

struct alignas(16) CameraBufferData final
{
	Math::Matrix4x4f worldToClipMatrix;

	Math::Vector3f cameraPosition;
	const float paddingCameraPos = -1.0f;

	Math::Vector2ui resolution;
	const float paddingResolution[2] = { -1.0f };
};

struct alignas(16) TransformBufferData final
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
	Math::Matrix4x4f directionalLightWorldToProjectionMatrix = Math::Matrix4x4f::Identity();
	Math::Vector4f directionalLightColor = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	Math::Vector3f directionalLightDirection;
	const float paddingDirectionalLightDirection = -1.0f;
};

struct alignas(16) JointsBufferData
{
	Math::Matrix4x4f bonesTransform[Graphics::Global_Max_Joints];
};