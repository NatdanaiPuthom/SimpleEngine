#pragma once
#include "../ScriptDefines.h"

namespace SCR
{
	struct Flow final
	{
		Flow(const bool aState = false)
			: myState(aState) {}

		operator bool() const
		{
			return myState;
		}

		inline static size_t typeID = typeid(Flow()).hash_code();
	private:

		bool myState;
	};

	inline Flow operator&&(Flow aFlow1, Flow aFlow2)
	{
		return aFlow1.operator bool() && aFlow2.operator bool();
	}

	inline Flow operator||(Flow aFlow1, Flow aFlow2)
	{
		return aFlow1.operator bool() || aFlow2.operator bool();
	}

	inline Flow operator!(Flow aFlow)
	{
		return !aFlow.operator bool();
	}

}