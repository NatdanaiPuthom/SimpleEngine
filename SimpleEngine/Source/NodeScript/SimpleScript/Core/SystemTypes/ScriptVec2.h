#pragma once
#include "../ScriptDefines.h"
#include <iostream>

namespace SCR
{
	struct ScriptVec2
	{
		float x = 0.f;
		float y = 0.f;
	};

	inline ScriptVec2 operator+(ScriptVec2 a, ScriptVec2 b)
	{
		return { a.x + b.x, a.y + b.y };
	}

	inline ScriptVec2 operator-(ScriptVec2 a, ScriptVec2 b)
	{
		return { a.x - b.x, a.y - b.y };
	}

	inline ScriptVec2 operator/(ScriptVec2 a, float b)
	{
		if (b != 0.f)
		{
			return { a.x / b, a.y / b };
		}
		return a;
	}
	inline void operator+=(ScriptVec2& a, ScriptVec2 b)
	{
		a = a + b;
	}

	inline void operator-=(ScriptVec2& a, ScriptVec2 b)
	{
		a = a - b;
	}

	inline void operator/=(ScriptVec2& a, float b)
	{
		a = a / b;
	}

	inline bool operator==(ScriptVec2 a, ScriptVec2 b)
	{
		return a.x == b.x && a.y == b.y;
	}

	inline bool operator!=(ScriptVec2 a, ScriptVec2 b)
	{
		return !(a == b);
	}

	inline std::ostream& operator<<(std::ostream& os, const ScriptVec2& a)
	{
		os << "{ X: " << a.x << ", Y: " << a.y << " }";
		return os;
	}
}