#pragma once

namespace FLY_NAMESPACE
{


	enum class eDataTypeTrait : unsigned int
	{
		None = 0,
		Fundamental = 1 << 0,
		ViewAndEditable = 1 << 1,
		Viewable = 1 << 2,
		SaveLoadable = 1 << 3,
		Targetable = 1 << 4,
		Pointer = 1 << 5,
		TriviallyCopyable = 1 << 6,
		GeneratedStruct = 1 << 7,
		GeneratedClass = 1 << 8,
		All = ~None
	};

}