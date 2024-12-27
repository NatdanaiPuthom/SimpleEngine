#pragma once
#include "../FlyDefines.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	struct VariableInstance final
	{
		OwningPtr<void> mDefaultValueDataPtr = nullptr;
		OwningPtr<void> mRuntimeValueDataPtr = nullptr;
	};

	class VariableContainer;


	class VariableContainerInstance final
	{
		static constexpr size_t MemoryAlignment = 1024;
	public:
		VariableContainerInstance() = default;
		VariableContainerInstance(const VariableContainer& aVariableContainer);
		~VariableContainerInstance();

		VariableContainerInstance(const VariableContainerInstance& aOther);
		VariableContainerInstance(VariableContainerInstance&&) noexcept = default;
		VariableContainerInstance& operator=(const VariableContainerInstance& aOther);
		VariableContainerInstance& operator=(VariableContainerInstance&&) noexcept = default;

		void Mirror();
		void InitRuntime();

		constexpr operator bool() const
		{
			return mVariableContainer != nullptr;
		}

		friend bool operator==(const VariableContainerInstance& a, const VariableContainerInstance& b);

	public:

		const VariableContainer* mVariableContainer = nullptr;
		std::vector<VariableInstance> mVariableInstances;
		MemoryArena<MemoryAlignment> mMemoryArena;
	};
	
}