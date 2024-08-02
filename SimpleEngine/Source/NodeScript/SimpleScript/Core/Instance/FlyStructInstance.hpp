#pragma once
#include "../FlyDefines.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	struct VariableInstance
	{
		OwningPtr<void> mRuntimeDataPtr = nullptr;
	};

	class Struct;

	constexpr size_t VariableArenaSize = 1024;

	class StructInstance final
	{
	public:

		StructInstance();
		~StructInstance();

		StructInstance(const StructInstance& aOther);
		StructInstance(StructInstance&&) noexcept = default;
		StructInstance& operator=(const StructInstance& aOther);
		StructInstance& operator=(StructInstance&&) noexcept = default;

		void Init(Struct& aStruct);

	
		std::vector<VariableInstance> mVariables;
		MemoryArena<VariableArenaSize> mMemoryArena;
	};
}