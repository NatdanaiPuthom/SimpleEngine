#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Lights/Light.hpp"

namespace Simple
{
	Light::Light(const Math::Transform& aTransform, const Math::Vector4f& aColor, const float aIntensity)
		: myTransform(aTransform)
		, myColor(aColor)
		, myIntensity(aIntensity)
	{
	}

	void Light::SetTransform(const Math::Transform& aTransform)
	{
		myTransform = aTransform;
	}

	void Light::SetPosition(const Math::Vector3f& aPosition)
	{
		myTransform.SetPosition(aPosition);
	}

	void Light::SetRotation(const Math::Vector3f& aRotation)
	{
		myTransform.SetRotation(aRotation);
	}

	void Light::SetColor(const Math::Vector4f& aColor)
	{
		myColor = aColor;
	}

	void Light::SetIntensity(const float aIntensity)
	{
		myIntensity = aIntensity;
	}

	Math::Transform Light::GetTransform() const
	{
		return myTransform;
	}

	Math::Vector4f Light::GetColor() const
	{
		return myColor;
	}

	float Light::GetIntensity() const
	{
		return  myIntensity;
	}
}