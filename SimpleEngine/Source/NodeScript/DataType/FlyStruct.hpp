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

		

	public:


		VariableContainer mVariableContainer;
		std::string mName;
	};
	
}