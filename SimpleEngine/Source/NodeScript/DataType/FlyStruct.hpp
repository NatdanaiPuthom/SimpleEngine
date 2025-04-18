#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableContainer.hpp"
#include <string>

namespace FLY_NAMESPACE
{

	class Struct final
	{
	public:

		Struct() = default;
		Struct(std::string_view aName);

		[[nodiscard]] std::string& Name();
		[[nodiscard]] const std::string& Name() const;

		[[nodiscard]] VariableContainer& GetVariableContainer();
		[[nodiscard]] const VariableContainer& GetVariableContainer() const;

	private:

		VariableContainer mVariableContainer;
		std::string mName;
	};

	inline VariableContainer& Struct::GetVariableContainer()
	{
		return mVariableContainer;
	}
	
	inline const VariableContainer& Struct::GetVariableContainer() const
	{
		return mVariableContainer;
	}
}