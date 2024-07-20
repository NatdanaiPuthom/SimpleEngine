#pragma once
#include "../ScriptDefines.hpp"
#include "../Pin/Pin.hpp"
#include "../Pin/PinType.hpp"

namespace SCR
{

	class Class;

	class ScriptFilter
	{
	public:

		ScriptFilter() = delete;
		~ScriptFilter();

		static std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph);
		static std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph);
		static std::vector<PinID> GetNonConnectedInputPins(const NodeGraph& aNodeGraph);
		static std::vector<PinID> GetNonConnectedOutputPins(const NodeGraph& aNodeGraph);
		static std::vector<PinID> GetNonConnectedPinsOfType(const NodeGraph& aNodeGraph, const eFlowType aFlowType);
		static std::vector<PinID> GetNonConnectedPinsOfTypeAndHash(const NodeGraph& aNodeGraph, const eFlowType aFlowType, const DataTypeID aDataTypeID);

	};
}