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

		Node(const NodeTypeID aTypeID, const auto& aInputPins, const auto& aOutputPins)
			: mTypeID(aTypeID)
			, mInputPins(begin(aInputPins), end(aInputPins))
			, mOutputPins(begin(aOutputPins), end(aOutputPins))
			, mSplitInputPins(mInputPins)
			, mSplitOutputPins(mOutputPins)
		{
		}

		Node(const NodeTypeID aTypeID, const std::vector<PinID>& aInputPins, const std::vector<PinID>& aOutputPins);

		[[nodiscard]] NodeTypeID GetTypeID() const;
		[[nodiscard]] std::vector<PinID>& GetInputPins();
		[[nodiscard]] const std::vector<PinID>& GetInputPins() const;
		[[nodiscard]] std::vector<PinID>& GetOutputPins();
		[[nodiscard]] const std::vector<PinID>& GetOutputPins() const;
		[[nodiscard]] std::vector<PinID>& GetSplitInputPins();
		[[nodiscard]] const std::vector<PinID>& GetSplitInputPins() const;
		[[nodiscard]] std::vector<PinID>& GetSplitOutputPins();
		[[nodiscard]] const std::vector<PinID>& GetSplitOutputPins() const;
		[[nodiscard]] Vec2 GetPosition() const;
		[[nodiscard]] bool IsDestroyed() const;

		void SetPosition(Vec2 aPosition);
		void SetIsDestroyed(bool aIsDestroyed);

	private:

		NodeTypeID mTypeID;
		std::vector<PinID> mInputPins;
		std::vector<PinID> mOutputPins;
		std::vector<PinID> mSplitInputPins;
		std::vector<PinID> mSplitOutputPins;
		Vec2 mPosition;
		bool mIsDestroyed = false;
	};

	inline NodeTypeID Node::GetTypeID() const
	{
		return mTypeID;
	}

	inline std::vector<PinID>& Node::GetInputPins()
	{
		return mInputPins;
	}

	inline const std::vector<PinID>& Node::GetInputPins() const
	{
		return mInputPins;
	}

	inline std::vector<PinID>& Node::GetOutputPins()
	{
		return mOutputPins;
	}

	inline const std::vector<PinID>& Node::GetOutputPins() const
	{
		return mOutputPins;
	}

	inline std::vector<PinID>& Node::GetSplitInputPins()
	{
		return mSplitInputPins;
	}

	inline const std::vector<PinID>& Node::GetSplitInputPins() const
	{
		return mSplitInputPins;
	}

	inline std::vector<PinID>& Node::GetSplitOutputPins()
	{
		return mSplitOutputPins;
	}

	inline const std::vector<PinID>& Node::GetSplitOutputPins() const
	{
		return mSplitOutputPins;
	}

}
