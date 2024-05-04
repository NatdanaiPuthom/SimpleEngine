#pragma once
#include "../ScriptDefines.h"

namespace SCR
{
	struct Undefined
	{
		inline Undefined& operator++()
		{
			return *this;
		}

		inline Undefined operator++(int)
		{
			Undefined temp(*this);
			return temp;
		}

		inline Undefined& operator--()
		{
			return *this;
		}

		inline Undefined operator--(int)
		{
			Undefined temp(*this);
			return temp;
		}

		inline void operator()()
		{
			
		}
	};

	inline bool operator==(const Undefined&, const Undefined&)
	{
		return false;
	}

	inline bool operator!=(const Undefined&, const Undefined&)
	{
		return false;
	}

	inline bool operator<=(const Undefined&, const Undefined&)
	{
		return false;
	}

	inline bool operator>=(const Undefined&, const Undefined&)
	{
		return false;
	}

	inline bool operator>(const Undefined&, const Undefined&)
	{
		return false;
	}

	inline bool operator<(const Undefined&, const Undefined&)
	{
		return false;
	}

	inline Undefined operator&&(const Undefined&, const Undefined&)
	{
		return Undefined();
	}

	inline Undefined operator||(const Undefined&, const Undefined&)
	{
		return Undefined();
	}

	inline Undefined operator!(const Undefined&)
	{
		return Undefined();
	}

	inline std::ostream& operator<<(std::ostream& aOStream, const Undefined&)
	{
		return aOStream;
	}

	inline Undefined operator+(const Undefined&, const Undefined&)
	{
		return Undefined();
	}

	inline Undefined operator-(const Undefined&, const Undefined&)
	{
		return Undefined();
	}

	inline Undefined operator*(const Undefined&, const Undefined&)
	{
		return Undefined();
	}

	inline Undefined operator/(const Undefined&, const Undefined&)
	{
		return Undefined();
	}

	inline Undefined operator%(const Undefined&, const Undefined&)
	{
		return Undefined();
	}
}