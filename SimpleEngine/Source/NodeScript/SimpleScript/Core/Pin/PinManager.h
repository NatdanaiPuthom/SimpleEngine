#pragma once
#include "../ScriptDefines.h"
#include "Pin.h"
#include "PinType.h"
#include "../DataStructures/ScriptTypeContainer.h"

namespace SCR
{

	class PinManager final
	{
		friend class ScriptProxy;
	public:

		PinManager();
		~PinManager();


		TypeContainer<PinID, const Pin, const PinType>::Iterator begin() const;
		TypeContainer<PinID, const Pin, const PinType>::Iterator end() const;

		std::vector<Pin> myPins;
	private:

	};

}