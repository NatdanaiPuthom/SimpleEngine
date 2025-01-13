#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/DataContainer/GraphicsDataContainer.hpp"

namespace Graphics
{
	GraphicsDataContainer::GraphicsDataContainer()
	{
	}

	GraphicsDataContainer::~GraphicsDataContainer()
	{
	}

	void GraphicsDataContainer::Init()
	{
		myLightBufferData = std::make_unique<LightBufferData>();
		myPostProcessData = std::make_unique<PostProcessData>();
		myPointLightBufferData = std::make_unique<PointLightBufferData>();

		myLightBufferData->directionalLightDirection.x = 0.0f;
		myLightBufferData->directionalLightDirection.y = -1.0f;
		myLightBufferData->directionalLightDirection.z = 0.0f;
	}

	void GraphicsDataContainer::ClearPointLightCount()
	{
		myPointLightBufferData->currentPointLightCount = 0;
	}

	void GraphicsDataContainer::AddPointLight(const PointLightData& aPointLightData)
	{
		myPointLightBufferData->pointLightData[myPointLightBufferData->currentPointLightCount] = aPointLightData;
		++myPointLightBufferData->currentPointLightCount;
	}

	void GraphicsDataContainer::SetAmbientLightColorAndIntensity(const Math::Vector4f& aColorAndIntensity)
	{
		myLightBufferData->ambientLightColorAndIntensity = aColorAndIntensity;
	}

	void GraphicsDataContainer::SetDirectionalLightColor(const Math::Vector4f& aColor)
	{
		myLightBufferData->directionalLightColorAndIntensity = aColor;
	}

	void GraphicsDataContainer::SetDirectionalLightDirection(const Math::Vector3f& aDirection)
	{
		myLightBufferData->directionalLightDirection = aDirection;
	}

	void GraphicsDataContainer::SetUseToneMapping(const bool aShouldUseToneMapping)
	{
		myPostProcessData->useToneMapping = aShouldUseToneMapping;
	}

	void GraphicsDataContainer::SetUseBloom(const bool aShouldUseBloom)
	{
		myPostProcessData->useBloom = aShouldUseBloom;
	}

	void GraphicsDataContainer::SetBloomPixelThreshold(const float aValue)
	{
		myPostProcessData->bloomPixelFilterThreshold = aValue;
	}

	void GraphicsDataContainer::SetSaturation(const float aValue)
	{
		myPostProcessData->saturation = aValue;
	}

	void GraphicsDataContainer::SetExposure(const float aValue)
	{
		myPostProcessData->exposure = aValue;
	}

	void GraphicsDataContainer::SetContrast(const float aValue)
	{
		myPostProcessData->contrast = aValue;
	}

	void GraphicsDataContainer::SetBlackPoint(const float aValue)
	{
		myPostProcessData->blackpoint = aValue;
	}

	void GraphicsDataContainer::SetBloom(const float aValue)
	{
		myPostProcessData->bloom = aValue;
	}

	void GraphicsDataContainer::SetTint(const Math::Vector3f& aColor)
	{
		myPostProcessData->tint = aColor;
	}

	LightBufferData* GraphicsDataContainer::GetLightBufferData()
	{
		return myLightBufferData.get();
	}

	PostProcessData* GraphicsDataContainer::GetPostProcessData()
	{
		return myPostProcessData.get();
	}

	Math::Vector4f GraphicsDataContainer::GetAmbientLightColorAndIntensity() const
	{
		return myLightBufferData->ambientLightColorAndIntensity;
	}

	Math::Vector4f GraphicsDataContainer::GetDirectionalLightColor() const
	{
		return myLightBufferData->directionalLightColorAndIntensity;
	}

	Math::Vector3f GraphicsDataContainer::GetDirectionalLightDirection() const
	{
		return myLightBufferData->directionalLightDirection;
	}

	PointLightData* GraphicsDataContainer::GetPointLightDataArray() const
	{
		return myPointLightBufferData->pointLightData;
	}

	PointLightBufferData* GraphicsDataContainer::GetPointLightBufferData()
	{
		return myPointLightBufferData.get();
	}

	size_t GraphicsDataContainer::GetPointLightCount() const
	{
		return myPointLightBufferData->currentPointLightCount;
	}
}
