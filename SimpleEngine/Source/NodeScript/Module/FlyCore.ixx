//export module FlyCore;
//
//import <algorithm>;
//import <iostream>;
//
//export namespace FlyInternal
//{
//	using NodeID = int;
//	using PinID = int;
//	using LinkID = int;
//
//	using NodeTypeID = size_t;
//	using CustomEventID = size_t;
//	using FunctionID = size_t;
//	using PinTypeID = size_t;
//	using VarID = size_t;
//	using MemoryPoolID = size_t;
//	using DataTypeID = size_t;
//	using ObjectTypeID = size_t;
//	using EventID = size_t;
//
//	template<std::integral IDType>
//	inline constexpr IDType InvalidID()
//	{
//		return std::numeric_limits<IDType>::max();
//	}
//
//	template<typename T>
//	inline constexpr DataTypeID GetDataTypeID()
//	{
//		return typeid(T).hash_code();
//	}
//
//	struct Vec2 final
//	{
//		float x = 0.f;
//		float y = 0.f;
//	};
//
//	constexpr inline Vec2 operator+(Vec2 a, Vec2 b)
//	{
//		return { a.x + b.x, a.y + b.y };
//	}
//
//	constexpr inline Vec2 operator-(Vec2 a, Vec2 b)
//	{
//		return { a.x - b.x, a.y - b.y };
//	}
//
//	constexpr inline Vec2 operator/(Vec2 a, float b)
//	{
//		if (b != 0.f)
//		{
//			return { a.x / b, a.y / b };
//		}
//		return a;
//	}
//	constexpr inline void operator+=(Vec2& a, Vec2 b)
//	{
//		a = a + b;
//	}
//
//	constexpr inline void operator-=(Vec2& a, Vec2 b)
//	{
//		a = a - b;
//	}
//
//	constexpr inline void operator/=(Vec2& a, float b)
//	{
//		a = a / b;
//	}
//
//	constexpr inline bool operator==(Vec2 a, Vec2 b)
//	{
//		return a.x == b.x && a.y == b.y;
//	}
//
//	constexpr inline bool operator!=(Vec2 a, Vec2 b)
//	{
//		return !(a == b);
//	}
//
//	inline std::ostream& operator<<(std::ostream& os, const Vec2& a)
//	{
//		os << "{ X: " << a.x << ", Y: " << a.y << " }";
//		return os;
//	}
//
//	// Struct for color - values between 0 and 1
//	struct Color final
//	{
//
//		Color() = default;
//		constexpr Color(float aR, float aG, float aB, float aA = 1.f)
//			: r(aR)
//			, g(aG)
//			, b(aB)
//			, a(aA)
//		{
//
//		}
//		float r = 0, g = 0, b = 0, a = 1;
//
//		constexpr void Clamp()
//		{
//			r = std::clamp(r, 0.f, 1.f);
//			g = std::clamp(g, 0.f, 1.f);
//			b = std::clamp(b, 0.f, 1.f);
//			a = std::clamp(a, 0.f, 1.f);
//		}
//
//	};
//
//	constexpr inline Color operator+(const Color& aColor1, const Color& aColor2)
//	{
//		Color c = { aColor1.r + aColor2.r, aColor1.g + aColor2.g, aColor1.b + aColor2.b, aColor1.a + aColor2.a };
//		c.Clamp();
//		return c;
//	}
//
//	constexpr inline Color operator-(const Color& aColor1, const Color& aColor2)
//	{
//		Color c = { aColor1.r - aColor2.r, aColor1.g - aColor2.g, aColor1.b - aColor2.b, aColor1.a - aColor2.a };
//		c.Clamp();
//		return c;
//	}
//
//	namespace Colors
//	{
//		constexpr Color Black = Color();
//		constexpr Color White = Color(1.f, 1.f, 1.f);
//		constexpr Color Red = Color(1.f, 0.f, 0.f);
//		constexpr Color Blue = Color(0.f, 0.f, 1.f);
//		constexpr Color Green = Color(0.f, 1.f, 0.f);
//		constexpr Color Yellow = Color(1.f, 1.f, 0.f);
//		constexpr Color Orange = Color(1.f, 0.65f, 0.f);
//		constexpr Color Pink = Color(1.f, 0.57f, 0.69f);
//		constexpr Color Purple = Color(0.6f, 0.f, 0.1f);
//		constexpr Color Gray = Color(0.5f, 0.5f, 0.5f);
//	}
//
//}