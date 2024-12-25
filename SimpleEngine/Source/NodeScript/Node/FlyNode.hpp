#pragma once
#include "../FlyDefines.hpp"
#include "../SystemTypes/FlyVec2.hpp"
#include "../Pin/FlyPin.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class Node final
	{
	public:

		template<typename IteratorType1, typename IteratorType2>
		Node(const NodeTypeID aTypeID, const IteratorType1& aInputPins, const IteratorType2& aOutputPins)
			: mTypeID(aTypeID)
			, mInputPins(std::vector<PinID>(aInputPins.begin(), aInputPins.end()))
			, mOutputPins(std::vector<PinID>(aOutputPins.begin(), aOutputPins.end()))
			, mSplitInputPins(mInputPins)
			, mSplitOutputPins(mOutputPins)
		{

		}

		Node(const NodeTypeID aTypeID, const std::vector<PinID>& aInputPins, const std::vector<PinID>& aOutputPins)
			: mTypeID(aTypeID)
			, mInputPins(aInputPins)
			, mOutputPins(aOutputPins)
			, mSplitInputPins(mInputPins)
			, mSplitOutputPins(mOutputPins)
		{

		}

		~Node() = default;

		Node(const Node&) = default;
		Node(Node&&) = default;
		Node& operator=(const Node&) = delete;
		Node& operator=(Node&&) = delete;

	public:


		const NodeTypeID mTypeID;
		std::vector<PinID> mInputPins;
		std::vector<PinID> mOutputPins;
		std::vector<PinID> mSplitInputPins;
		std::vector<PinID> mSplitOutputPins;

		Vec2 mPosition;
		bool mIsDestroyed = false;
		
	};

	
}
