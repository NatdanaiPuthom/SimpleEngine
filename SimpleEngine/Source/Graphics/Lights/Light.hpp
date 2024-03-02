#pragma once
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vector4.hpp"

namespace Simple
{
	class Light
	{
	public:
		Light(const Math::Transform& aTransform, const Math::Vector4f& aColor, const float aIntensity);
		virtual ~Light() = default;

	protected:
	};
}