#pragma once
#include "../ScriptDefines.h"
#include "../SystemTypes/ScriptVec2.h"
#include <vector>

namespace SCR
{

	class Node
	{
	public:

		template<typename IteratorType1, typename IteratorType2>
		Node(const NodeTypeID aTypeID, const IteratorType1& aInputPins, const IteratorType2& aOutputPins)
			: typeID(aTypeID)
			, inputPins(std::vector<PinID>(aInputPins.begin(), aInputPins.end()))
			, outputPins(std::vector<PinID>(aOutputPins.begin(), aOutputPins.end()))
		{

		}

		Node(const NodeTypeID aTypeID, const std::vector<PinID>& aInputPins, const std::vector<PinID>& aOutputPins)
			: typeID(aTypeID)
			, inputPins(aInputPins)
			, outputPins(aOutputPins)
		{

		}

		~Node() = default;

		Node(const Node&) = delete;
		Node(Node&&) = default;
		Node& operator=(const Node&) = delete;
		Node& operator=(Node&&) = default;

	public:


		const NodeTypeID typeID;
		std::vector<PinID> inputPins;
		std::vector<PinID> outputPins;

		ScriptVec2 position;
		bool isDestroyed = false;

		
	};

	
}
