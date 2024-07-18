#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Defines.hpp"

namespace Graphics
{
	struct alignas(16) PointLightData final
	{
		Math::Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Math::Vector3f position = { 0.0f, 0.0f, 0.0f }; //NOTE(v11.3.4): Will be updated to sync with Entity's TransformComponent. May fix a better way in future
		float radius = 0.0f;
	};
}

struct alignas(16) PostProcessData final
{ //NOTE(v10.0.5): SimpleEngine default value
	Math::Vector3f tint = { 1.0f, 1.0f, 1.0f };
	float saturation = 1.070f;

	float exposure = 0.060f;
	float contrast = 0.970f;
	float blackpoint = 0.020f;
	float bloom = 1.280f;

	float bloomPixelFilterThreshold = 0.991f;
	unsigned int useToneMapping = 0;
	unsigned int useBloom = 0;
	const int paddingPostProcessData = -1;
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
	Math::Vector4f ambientLightColorAndIntensity = { 1.0f, 1.0f, 1.0f, 1.0f };
	Math::Vector4f directionalLightColorAndIntensity = { 1.0f, 1.0f, 1.0f, 1.0f };

	Math::Vector3f directionalLightDirection = { 0.0f, 0.0f, 1.0f };
	const float paddingDirectionalLightDirection = -1.0f;
};

struct alignas(16) PointLightBufferData final
{
	Graphics::PointLightData pointLightData[Graphics::Global_Max_PointLights];

	unsigned int currentPointLightCount = 0;
	const float paddingPointLightCount[3] = { -1.0f };
};

struct alignas(16) JointsBufferData final
{
	Math::Matrix4x4f bonesTransform[Graphics::Global_Max_Joints];
};