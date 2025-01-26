#pragma once
#include "Engine/SimpleUtilities/Sphere.hpp"
#include "Engine/SimpleUtilities/AABB3D.hpp"
#include "Engine/SimpleUtilities/Ray.hpp"
#include <variant>

namespace Simple
{
	using CollisionShape = std::variant<Sphere, AABB3D, Ray>;

}