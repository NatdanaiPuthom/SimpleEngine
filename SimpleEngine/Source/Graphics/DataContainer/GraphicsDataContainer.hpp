#pragma once
#include "Graphics/BufferData.hpp"
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
		void SetAmbientLightColorAndIntensity(const Math::Vector4f& aColorAndIntensity);
		void SetDirectionalLightColor(const Math::Vector4f& aColor);
		void SetDirectionalLightDirection(const Math::Vector3f& aDirection);
	public:
		LightBufferData* GetLightBufferData();
		Math::Vector4f GetAmbientLightColorAndIntensity() const;
		Math::Vector4f GetDirectionalLightColor() const;
		Math::Vector3f GetDirectionalLightDirection() const;
	private:
		std::unique_ptr<LightBufferData> myLightBufferData;
	};
}
