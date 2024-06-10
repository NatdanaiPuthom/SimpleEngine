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

		PinManager(const PinManager&);


		TypeContainer<PinID, Pin, PinType>::ConstIterator begin() const;
		TypeContainer<PinID, Pin, PinType>::ConstIterator end() const;

		std::vector<Pin> myPins;
	private:

	};

}