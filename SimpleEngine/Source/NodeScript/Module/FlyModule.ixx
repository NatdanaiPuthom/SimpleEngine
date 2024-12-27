export module MOD_NodeScript_FlyModule;

import MOD_NodeScript_FlyCore;

namespace FlyInternal
{
	export class NodeEmilTest
	{
	public:
		int x = 0;
	};
}
//
//import FlyCore;
//
//import <vector>;
//
//export namespace FlyInternal
//{
//
//	class Node final
//	{
//	public:
//
//		template<typename IteratorType1, typename IteratorType2>
//		Node(const NodeTypeID aTypeID, const IteratorType1& aInputPins, const IteratorType2& aOutputPins)
//			: mTypeID(aTypeID)
//			, mInputPins(std::vector<PinID>(aInputPins.begin(), aInputPins.end()))
//			, mOutputPins(std::vector<PinID>(aOutputPins.begin(), aOutputPins.end()))
//		{
//
//		}
//
//		Node(const NodeTypeID aTypeID, const std::vector<PinID>& aInputPins, const std::vector<PinID>& aOutputPins)
//			: mTypeID(aTypeID)
//			, mInputPins(aInputPins)
//			, mOutputPins(aOutputPins)
//		{
//
//		}
//
//		~Node() = default;
//
//		Node(const Node&) = default;
//		Node(Node&&) = default;
//		Node& operator=(const Node&) = delete;
//		Node& operator=(Node&&) = delete;
//
//	public:
//
//
//		const NodeTypeID mTypeID;
//		std::vector<PinID> mInputPins;
//		std::vector<PinID> mOutputPins;
//
//		Vec2 mPosition;
//		bool mIsDestroyed = false;
//
//	};
//}