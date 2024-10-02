#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	struct Wildcard final
	{
		inline Wildcard& operator++()
		{
			return *this;
		}

		inline Wildcard operator++(int)
		{
			Wildcard temp(*this);
			return temp;
		}

		inline Wildcard& operator--()
		{
			return *this;
		}

		inline Wildcard operator--(int)
		{
			Wildcard temp(*this);
			return temp;
		}

		inline void operator()()
		{
			
		}
	};

	inline bool operator==(const Wildcard&, const Wildcard&)
	{
		return false;
	}

	inline bool operator!=(const Wildcard&, const Wildcard&)
	{
		return false;
	}

	inline bool operator<=(const Wildcard&, const Wildcard&)
	{
		return false;
	}

	inline bool operator>=(const Wildcard&, const Wildcard&)
	{
		return false;
	}

	inline bool operator>(const Wildcard&, const Wildcard&)
	{
		return false;
	}

	inline bool operator<(const Wildcard&, const Wildcard&)
	{
		return false;
	}

	inline Wildcard operator&&(const Wildcard&, const Wildcard&)
	{
		return Wildcard();
	}

	inline Wildcard operator||(const Wildcard&, const Wildcard&)
	{
		return Wildcard();
	}

	inline Wildcard operator!(const Wildcard&)
	{
		return Wildcard();
	}

	inline std::ostream& operator<<(std::ostream& aOStream, const Wildcard&)
	{
		return aOStream;
	}

	inline Wildcard operator+(const Wildcard&, const Wildcard&)
	{
		return Wildcard();
	}

	inline Wildcard operator-(const Wildcard&, const Wildcard&)
	{
		return Wildcard();
	}

	inline Wildcard operator*(const Wildcard&, const Wildcard&)
	{
		return Wildcard();
	}

	inline Wildcard operator/(const Wildcard&, const Wildcard&)
	{
		return Wildcard();
	}

	inline Wildcard operator%(const Wildcard&, const Wildcard&)
	{
		return Wildcard();
	}
}