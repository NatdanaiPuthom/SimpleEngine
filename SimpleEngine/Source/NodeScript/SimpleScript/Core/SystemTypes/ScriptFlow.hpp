#pragma once
#include "../ScriptDefines.hpp"

namespace SCR
{
	struct Flow final
	{
		explicit Flow(const bool aState = false)
			: myState(aState) {}

		operator bool() const
		{
			return myState;
		}

		inline static size_t mTypeID = typeid(int).hash_code();
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