#pragma once
#include <limits>
#include <functional>

namespace Simple
{
	struct EntityID final
	{
		size_t value;

		explicit EntityID(const size_t aValue) : value(aValue) {}

		operator size_t() const { return value; }

		bool operator==(const EntityID& aOther) const { return value == aOther.value; }
		bool operator!=(const EntityID& aOther) const { return value != aOther.value; }

		static constexpr size_t Invalid = std::numeric_limits<size_t>::max();

		struct Hash
		{
			size_t operator()(const EntityID& aEntityID) const { return std::hash<size_t>{}(aEntityID.value); }
		};
	};
}