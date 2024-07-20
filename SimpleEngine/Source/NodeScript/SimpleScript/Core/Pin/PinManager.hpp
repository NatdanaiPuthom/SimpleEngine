#pragma once
#include "../ScriptDefines.hpp"
#include "Pin.hpp"

namespace SCR
{

	class PinManager final
	{
		friend class ScriptProxy;
	public:

		PinManager();
		~PinManager();

		std::vector<Pin> myPins;
	private:

	};

}