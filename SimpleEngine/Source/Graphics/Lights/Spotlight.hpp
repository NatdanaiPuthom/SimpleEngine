#pragma once
#include "Graphics/Lights/Light.hpp"

namespace Simple
{
	class SpotLight final : public Light
	{
	public:
		SpotLight();
		SpotLight(const Math::Transform& aTransform, const Math::Vector4f& aColor, const float aIntensity = 1.0f, const float aRange = 10.0f, const float aInnerAngleInDegree = 30.0f, const float aOuterAngleInDegree = 60.0f);
		virtual ~SpotLight() = default;
	public:
		void SetRange(const float aRange);
		void SetInnerAngle(const float aInnerAngleInDegree);
		void SetOuterAngle(const float aOuterAngleInDegree);
		void SetLightDirection(const Math::Vector3f& aDirection);
	public:
		float GetRange() const;
		float GetInnerAngleInDegree() const;
		float GetOuterAngleInDegree() const;
		Math::Vector3f GetLightDirection() const;
	private:
		Math::Vector3f myLightDirection;
		float myRange;
		float myInnerAngleInDegree;
		float myOuterAngleInDegree;
	};
}