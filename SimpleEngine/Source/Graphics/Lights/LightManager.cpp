#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Lights/LightManager.hpp"

namespace Graphics
{
	LightManager::LightManager()
	{
	}

	LightManager::~LightManager()
	{
	}

	void LightManager::Init()
	{
		myLightBufferData = std::make_unique<LightBufferData>();
		myPostProcessData = std::make_unique<PostProcessData>();
		myPointLightBufferData = std::make_unique<PointLightBufferData>();

		myLightBufferData->directionalLightDirection.x = 0.0f;
		myLightBufferData->directionalLightDirection.y = -1.0f;
		myLightBufferData->directionalLightDirection.z = 0.0f;
	}

	void LightManager::ClearPointLightCount()
	{
		myPointLightBufferData->currentPointLightCount = 0;
	}

	void LightManager::AddPointLight(const PointLightData& aPointLightData)
	{
		myPointLightBufferData->pointLightData[myPointLightBufferData->currentPointLightCount] = aPointLightData;
		++myPointLightBufferData->currentPointLightCount;
	}

	void LightManager::SetAmbientLightColorAndIntensity(const Math::Vector4f& aColorAndIntensity)
	{
		myLightBufferData->ambientLightColorAndIntensity = aColorAndIntensity;
	}

	void LightManager::SetDirectionalLightColor(const Math::Vector4f& aColor)
	{
		myLightBufferData->directionalLightColorAndIntensity = aColor;
	}

	void LightManager::SetDirectionalLightDirection(const Math::Vector3f& aDirection)
	{
		myLightBufferData->directionalLightDirection = aDirection;
	}

	void LightManager::SetUseToneMapping(const bool aShouldUseToneMapping)
	{
		myPostProcessData->useToneMapping = aShouldUseToneMapping;
	}

	void LightManager::SetUseBloom(const bool aShouldUseBloom)
	{
		myPostProcessData->useBloom = aShouldUseBloom;
	}

	void LightManager::SetBloomPixelThreshold(const float aValue)
	{
		myPostProcessData->bloomPixelFilterThreshold = aValue;
	}

	void LightManager::SetSaturation(const float aValue)
	{
		myPostProcessData->saturation = aValue;
	}

	void LightManager::SetExposure(const float aValue)
	{
		myPostProcessData->exposure = aValue;
	}

	void LightManager::SetContrast(const float aValue)
	{
		myPostProcessData->contrast = aValue;
	}

	void LightManager::SetBlackPoint(const float aValue)
	{
		myPostProcessData->blackpoint = aValue;
	}

	void LightManager::SetBloom(const float aValue)
	{
		myPostProcessData->bloom = aValue;
	}

	void LightManager::SetTint(const Math::Vector3f& aColor)
	{
		myPostProcessData->tint = aColor;
	}

	LightBufferData* LightManager::GetLightBufferData()
	{
		return myLightBufferData.get();
	}

	PostProcessData* LightManager::GetPostProcessData()
	{
		return myPostProcessData.get();
	}

	Math::Vector4f LightManager::GetAmbientLightColorAndIntensity() const
	{
		return myLightBufferData->ambientLightColorAndIntensity;
	}

	Math::Vector4f LightManager::GetDirectionalLightColor() const
	{
		return myLightBufferData->directionalLightColorAndIntensity;
	}

	Math::Vector3f LightManager::GetDirectionalLightDirection() const
	{
		return myLightBufferData->directionalLightDirection;
	}

	PointLightData* LightManager::GetPointLightDataArray() const
	{
		return myPointLightBufferData->pointLightData;
	}

	PointLightBufferData* LightManager::GetPointLightBufferData()
	{
		return myPointLightBufferData.get();
	}

	size_t LightManager::GetPointLightCount() const
	{
		return myPointLightBufferData->currentPointLightCount;
	}
}
