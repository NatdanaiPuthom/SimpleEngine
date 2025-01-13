#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include <memory>

namespace Graphics
{
	struct alignas(16) PostProcessData final
	{
		Math::Vector3f tint = { 1.0f, 1.0f, 1.0f };
		float saturation = 1.070f;

		float exposure = 0.060f;
		float contrast = 0.970f;
		float blackpoint = 0.020f;
		float bloom = 1.280f;

		float bloomPixelFilterThreshold = 0.991f;
		unsigned int useToneMapping = 0;
		unsigned int useBloom = 0;
		unsigned int paddingPostProcessData = 0;
	};

	struct alignas(16) LightBufferData final
	{
		Math::Vector4f ambientLightColorAndIntensity = { 1.0f, 1.0f, 1.0f, 1.0f };
		Math::Vector4f directionalLightColorAndIntensity = { 1.0f, 1.0f, 1.0f, 1.0f };

		Math::Vector3f directionalLightDirection = { 0.0f, 0.0f, 1.0f };
		const float paddingDirectionalLightDirection = -1.0f;
	};
}

namespace Graphics
{
	class GraphicsDataContainer
	{
	public:
		GraphicsDataContainer();
		~GraphicsDataContainer();

		void Init();
	public:
		void SetAmbientLightColorAndIntensity(const Math::Vector4f& aColorAndIntensity);
		void SetDirectionalLightColor(const Math::Vector4f& aColor);
		void SetDirectionalLightDirection(const Math::Vector3f& aDirection);
		void SetUseToneMapping(const bool aShouldUseToneMapping);
		void SetUseBloom(const bool aShouldUseBloom);
		void SetBloomPixelThreshold(const float aValue);
		void SetSaturation(const float aValue);
		void SetExposure(const float aValue);
		void SetContrast(const float aValue);
		void SetBlackPoint(const float aValue);
		void SetBloom(const float aValue);
		void SetTint(const Math::Vector3f& aColor);
	public:
		LightBufferData* GetLightBufferData();
		PostProcessData* GetPostProcessData();
		Math::Vector4f GetAmbientLightColorAndIntensity() const;
		Math::Vector4f GetDirectionalLightColor() const;
		Math::Vector3f GetDirectionalLightDirection() const;
	private:
		std::unique_ptr<LightBufferData> myLightBufferData;
		std::unique_ptr<PostProcessData> myPostProcessData;
	};
}
