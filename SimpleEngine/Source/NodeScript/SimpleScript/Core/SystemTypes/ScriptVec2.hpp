#pragma once
#include "../ScriptDefines.hpp"
#include <iostream>

namespace FLY_NAMESPACE
{
	struct Vec2
	{
		float x = 0.f;
		float y = 0.f;
	};

	inline Vec2 operator+(Vec2 a, Vec2 b)
	{
		return { a.x + b.x, a.y + b.y };
	}

	inline Vec2 operator-(Vec2 a, Vec2 b)
	{
		return { a.x - b.x, a.y - b.y };
	}

	inline Vec2 operator/(Vec2 a, float b)
	{
		if (b != 0.f)
		{
			return { a.x / b, a.y / b };
		}
		return a;
	}
	inline void operator+=(Vec2& a, Vec2 b)
	{
		a = a + b;
	}

	inline void operator-=(Vec2& a, Vec2 b)
	{
		a = a - b;
	}

	inline void operator/=(Vec2& a, float b)
	{
		a = a / b;
	}

	inline bool operator==(Vec2 a, Vec2 b)
	{
		return a.x == b.x && a.y == b.y;
	}

	inline bool operator!=(Vec2 a, Vec2 b)
	{
		return !(a == b);
	}

	inline std::ostream& operator<<(std::ostream& os, const Vec2& a)
	{
		os << "{ X: " << a.x << ", Y: " << a.y << " }";
		return os;
	}
}