#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariable.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class Struct final
	{
		static constexpr size_t StructAlignment = 512;
	public:

		Struct() = default;
		~Struct() = default;

		Struct(const Struct&) = delete;
		Struct(Struct&&) = default;
		Struct& operator=(const Struct&) = delete;
		Struct& operator=(Struct&&) = default;

	public:

		std::vector<Variable> mVariables;

		MemoryArena<StructAlignment> mMemoryArena;

	};
}