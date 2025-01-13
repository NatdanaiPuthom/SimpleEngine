#pragma once
#include "Graphics/BufferData.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include <memory>

namespace Graphics
{
	class GraphicsDataContainer
	{
	public:
		GraphicsDataContainer();
		~GraphicsDataContainer();

		void Init();
	public:
		void ClearPointLightCount();
	public:
		void AddPointLight(const PointLightData& aPointLightData);
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
		PointLightData* GetPointLightDataArray() const;
		PointLightBufferData* GetPointLightBufferData();
		Math::Vector4f GetAmbientLightColorAndIntensity() const;
		Math::Vector4f GetDirectionalLightColor() const;
		Math::Vector3f GetDirectionalLightDirection() const;
		size_t GetPointLightCount() const;
	private:
		std::unique_ptr<LightBufferData> myLightBufferData;
		std::unique_ptr<PostProcessData> myPostProcessData;
		std::unique_ptr<PointLightBufferData> myPointLightBufferData;
	};
}
