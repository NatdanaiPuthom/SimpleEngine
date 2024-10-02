#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
{
	class CustomEvent final
	{
	public:

		CustomEvent(std::string_view aName);
		~CustomEvent();

		inline const NodeTypeID& GetExecutorTypeID() const
		{
			return mExecutorTypeID;
		}

		inline const NodeTypeID& GetCallerTypeID() const
		{
			return mCallerTypeID;
		}

	private:

		NodeTypeID mExecutorTypeID;
		NodeTypeID mCallerTypeID;
	};
}