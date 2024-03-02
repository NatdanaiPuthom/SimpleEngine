#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"

namespace Simple
{
	class Light
	{
	public:
		Light(const Math::Transform& aTransform, const Math::Vector4f& aColor, const float aIntensity);
		virtual ~Light() = default;
	public:
		void SetTransform(const Math::Transform& aTransform);
		void SetPosition(const Math::Vector3f& aPosition);
		void SetRotation(const Math::Vector3f& aRotation);
		void SetColor(const Math::Vector4f& aColor);
		void SetIntensity(const float aIntensity);
	public:
		Math::Transform GetTransform() const;
		Math::Vector4f GetColor() const;
		float GetIntensity() const;
	protected:
		Math::Transform myTransform;
		Math::Vector4f myColor;
		float myIntensity;
	};
}