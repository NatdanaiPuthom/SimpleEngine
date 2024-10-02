#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	struct Flow final
	{
		explicit Flow(const bool aState = false)
			: myState(aState) {}

		operator bool() const
		{
			return myState;
		}

		static DataTypeID mTypeID;
	private:

		bool myState;
	};

	inline Flow operator&&(Flow aFlow1, Flow aFlow2)
	{
		return Flow(aFlow1.operator bool() && aFlow2.operator bool());
	}

	inline Flow operator||(Flow aFlow1, Flow aFlow2)
	{
		return Flow(aFlow1.operator bool() || aFlow2.operator bool());
	}

	inline Flow operator!(Flow aFlow)
	{
		return Flow(!aFlow.operator bool());
	}

}