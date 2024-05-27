#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"

struct alignas(16) PointLightData final
{
	Math::Vector4f position = { 0.0f, 0.0f, 0.0f, 1.0f };
	Math::Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
	float range = 0.0f;
	float padding[3] = { -1.0f };
};

struct alignas(16) CameraBufferData final
{
	Math::Matrix4x4f worldToClipMatrix = Math::Matrix4x4f::Identity();

	Math::Vector3f cameraPosition = { 0.0f, 0.0f, 0.0f };
	const float paddingCameraPos = -1.0f;

	Math::Vector2ui resolution;
	const float paddingResolution[2] = { -1.0f };
};

struct alignas(16) TransformBufferData final
{
	Math::Matrix4x4f modelWorldMatrix = Math::Matrix4x4f::Identity();
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
	Math::Vector4f ambientLightColorAndIntensity = { 1.0f, 1.0f, 1.0f, 1.0f };
	Math::Vector4f directionalLightColorAndIntensity = { 1.0f, 1.0f, 1.0f, 1.0f };

	Math::Vector3f directionalLightDirection = { 0.0f, 0.0f, 1.0f };
	const float paddingDirectionalLightDirection = -1.0f;

	PointLightData pointLightData[8];

	unsigned int currentPointLightCount = 0;
	const float paddingPointLightCount[3] = { -1.0f };
};

struct alignas(16) JointsBufferData
{
	Math::Matrix4x4f bonesTransform[Graphics::Global_Max_Joints];
};