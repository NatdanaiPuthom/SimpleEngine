#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Lights/Light.hpp"

Simple::Light::Light(const Math::Transform& aTransform, const Math::Vector4f& aColor, const float aIntensity)
	: myTransform(aTransform)
	, myColor(aColor)
	, myIntensity(aIntensity)
{
}

void Simple::Light::SetTransform(const Math::Transform& aNewTransform)
{
	myTransform = aNewTransform;
}

void Simple::Light::SetPosition(const Math::Vector3f& aNewPosition)
{
	myTransform.SetPosition(aNewPosition);
}

void Simple::Light::SetRotation(const Math::Vector3f& aNewRotation)
{
	myTransform.SetRotation(aNewRotation);
}

void Simple::Light::SetColor(const Math::Vector4f& aNewColor)
{
	myColor = aNewColor;
}

void Simple::Light::SetIntensity(const float aNewIntensity)
{
	myIntensity = aNewIntensity;
}

Math::Transform Simple::Light::GetTransform() const
{
	return myTransform;
}

Math::Vector4f Simple::Light::GetColor() const
{
	return myColor;
}

float Simple::Light::GetIntensity() const
{
	return  myIntensity;
}