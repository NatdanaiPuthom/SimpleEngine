#pragma once
#include "../FlyDefines.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	struct VariableInstance final
	{
		OwningPtr<void> mDefaultValueDataPtr = nullptr;
		OwningPtr<void> mRuntimeValueDataPtr = nullptr;
	};

	class Struct;


	class StructInstance final
	{
		static constexpr size_t MemoryAlignment = 1024;
	public:
		StructInstance(const Struct& aStruct);
		~StructInstance();

		StructInstance(const StructInstance& aOther);
		StructInstance(StructInstance&&) noexcept = default;
		StructInstance& operator=(const StructInstance& aOther);
		StructInstance& operator=(StructInstance&&) noexcept = default;

		void Mirror();
		void InitRuntime();

		const Struct* mStruct;
		std::vector<VariableInstance> mVariableInstances;
		MemoryArena<MemoryAlignment> mMemoryArena;
	};
}