#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	class Link final
	{
	public:

		Link(PinID aInputPinID, PinID aOutputPinID);

		[[nodiscard]] PinID GetInputPinID() const;
		[[nodiscard]] PinID GetOutputPinID() const;
		void SetIsDestroyed(bool aIsDestroyed);
		[[nodiscard]] bool IsDestroyed() const;

		friend bool operator==(const Link& a, const Link& b);

		explicit operator bool() const
		{
			return mInputPinID != InvalidID<PinID>() && mOutputPinID != InvalidID<PinID>();
		}

	private:

		PinID mInputPinID = InvalidID<PinID>();
		PinID mOutputPinID = InvalidID<PinID>();
		bool mIsDestroyed = false;
	};

	inline bool operator==(const Link& a, const Link& b)
	{
		return a.mInputPinID == b.mInputPinID && a.mOutputPinID == b.mOutputPinID;
	}
}