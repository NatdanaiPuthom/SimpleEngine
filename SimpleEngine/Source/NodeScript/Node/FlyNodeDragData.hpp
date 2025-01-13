#pragma once
#include "../FlyDefines.hpp"
#include "../SystemTypes/FlyVec2.hpp"

namespace FLY_NAMESPACE
{
	struct NodeDragData final
	{
		Vec2 mStartPos;
		Vec2 mEndPos;
	};
}