#pragma once
#include "../FlyDefines.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include "FlyVariable.hpp"
#include <vector>

namespace FLY_NAMESPACE
{
	class VariableContainer final
	{
		static constexpr size_t Alignment = 512;
	public:

		VariableContainer() = default;
		~VariableContainer() = default;

		VariableContainer(const VariableContainer& aOther);
		VariableContainer(VariableContainer&&) = default;
		VariableContainer& operator=(const VariableContainer& aOther);
		VariableContainer& operator=(VariableContainer&&) = default;

		[[nodiscard]] const std::vector<Variable>& GetVariables();
		[[nodiscard]] MemoryArena<Alignment>& GetMemoryArena();
		[[nodiscard]] const MemoryArena<Alignment>& GetMemoryArena() const;
		[[nodiscard]] Variable& GetVariable(size_t aIndex);
		[[nodiscard]] const Variable& GetVariable(size_t aIndex) const;
		[[nodiscard]] size_t GetVariableCount() const;
		[[nodiscard]] std::vector<Variable>::iterator begin();
		[[nodiscard]] std::vector<Variable>::const_iterator begin() const;
		[[nodiscard]] std::vector<Variable>::iterator end();
		[[nodiscard]] std::vector<Variable>::const_iterator end() const;

		void AddVariable(Variable aVariable);

	private:

		std::vector<Variable> mVariables;
		MemoryArena<Alignment> mMemoryArena;
	};
	
	inline const std::vector<Variable>& VariableContainer::GetVariables()
	{
		return mVariables;
	}

	inline MemoryArena<VariableContainer::Alignment>& VariableContainer::GetMemoryArena()
	{
		return mMemoryArena;
	}

	inline const MemoryArena<VariableContainer::Alignment>& VariableContainer::GetMemoryArena() const
	{
		return mMemoryArena;
	}

	inline Variable& VariableContainer::GetVariable(size_t aIndex)
	{
		return mVariables[aIndex];
	}

	inline const Variable& VariableContainer::GetVariable(size_t aIndex) const
	{
		return mVariables[aIndex];
	}

	inline size_t VariableContainer::GetVariableCount() const
	{
		return mVariables.size();
	}

	inline std::vector<Variable>::iterator VariableContainer::begin()
	{
		return mVariables.begin();
	}

	inline std::vector<Variable>::const_iterator VariableContainer::begin() const
	{
		return mVariables.begin();
	}

	inline std::vector<Variable>::iterator VariableContainer::end()
	{
		return mVariables.end();
	}

	inline std::vector<Variable>::const_iterator VariableContainer::end() const
	{
		return mVariables.end();
	}
}