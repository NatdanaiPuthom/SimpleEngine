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

		myLightBufferData->directionalLightDirection.x = 0.0f;
		myLightBufferData->directionalLightDirection.y = -1.0f;
		myLightBufferData->directionalLightDirection.z = 0.0f;
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

	LightBufferData* GraphicsDataContainer::GetLightBufferData()
	{
		return myLightBufferData.get();
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
}
