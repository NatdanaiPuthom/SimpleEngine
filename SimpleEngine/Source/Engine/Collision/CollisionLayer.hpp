#pragma once

namespace Simple
{

	enum class eCollisionLayer : uint8_t
	{
		None = 0,
		Player = 1 << 0,
		Enemy = 1 << 1,
		Wall = 1 << 2,
		Interactable = 1 << 3,
		All = 0xFF
	};
	

	constexpr eCollisionLayer operator|(const eCollisionLayer a, const eCollisionLayer b)
	{
		return static_cast<eCollisionLayer>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
	}

	constexpr eCollisionLayer operator&(const eCollisionLayer a, const eCollisionLayer b)
	{
		return static_cast<eCollisionLayer>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
	}



}