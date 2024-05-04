#pragma once
#include "../ScriptDefines.h"
#include "../Pin/Pin.h"
#include "../Pin/PinType.h"

namespace SCR
{

	class Script;

	class ScriptFilter
	{
	public:

		ScriptFilter(const Script& aScript);
		~ScriptFilter();

		std::vector<PinID> GetInputPins() const;
		std::vector<PinID> GetOutputPins() const;
		std::vector<PinID> GetNonConnectedInputPins() const;
		std::vector<PinID> GetNonConnectedOutputPins() const;
		std::vector<PinID> GetNonConnectedPinsOfType(const ePinFlowType aFlowType) const;
		std::vector<PinID> GetNonConnectedPinsOfTypeAndHash(const ePinFlowType aFlowType, const DataTypeID aDataTypeID) const;

	private:

		const Script& myScript;
	};
}