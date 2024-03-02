#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Lights/Spotlight.hpp"

namespace Simple
{
	SpotLight::SpotLight()
		: Light(Math::Transform(), {1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)
		, myRange(10.0f)
		, myInnerAngleInDegree(30.0f)
		, myOuterAngleInDegree(60.0f)
	{
	}

	SpotLight::SpotLight(const Math::Transform& aTransform, const Math::Vector4f& aColor, const float aIntensity, const float aRange, const float aInnerAngleInDegree, const float aOuterAngleInDegree)
		: Light(aTransform, aColor, aIntensity)
		, myRange(aRange)
		, myInnerAngleInDegree(aInnerAngleInDegree)
		, myOuterAngleInDegree(aOuterAngleInDegree)
	{
	}

	void SpotLight::SetRange(const float aRange)
	{
		myRange = aRange;
	}
	void SpotLight::SetInnerAngle(const float aInnerAngleInDegree)
	{
		myInnerAngleInDegree = aInnerAngleInDegree;
	}

	void SpotLight::SetOuterAngle(const float aOuterAngleInDegree)
	{ 
		myOuterAngleInDegree = aOuterAngleInDegree;
	}

	void SpotLight::SetLightDirection(const Math::Vector3f& aDirection)
	{
		myLightDirection = aDirection;
	}

	float SpotLight::GetRange() const
	{
		return myRange;
	}

	float SpotLight::GetInnerAngleInDegree() const
	{
		return myInnerAngleInDegree;
	}

	float SpotLight::GetOuterAngleInDegree() const
	{
		return myOuterAngleInDegree;
	}

	Math::Vector3f SpotLight::GetLightDirection() const
	{
		return myLightDirection;
	}
}