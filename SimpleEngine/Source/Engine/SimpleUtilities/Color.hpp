#pragma once

namespace Simple
{

	class Color final
	{
	public:

		constexpr Color() = default;
		constexpr Color(const float aR, const float aG, const float aB, const float aA)
			: r(aR)
			, g(aG)
			, b(aB)
			, a(aA)
		{
		}

		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 1.f;

		friend bool operator==(const Color& a, const Color& b)
		{
			return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
		}
	};

	namespace Colors
	{
		inline static constexpr Color Black = Color(0.f, 0.f, 0.f, 1.f);
		inline static constexpr Color Red = Color(1.f, 0.f, 0.f, 1.f);
		inline static constexpr Color Green = Color(0.f, 1.f, 0.f, 1.f);
		inline static constexpr Color Blue = Color(0.f, 0.f, 1.f, 1.f);
		inline static constexpr Color Yellow = Color(1.f, 1.f, 0.f, 1.f);
		inline static constexpr Color White = Color(1.f, 1.f, 1.f, 1.f);
	}
}