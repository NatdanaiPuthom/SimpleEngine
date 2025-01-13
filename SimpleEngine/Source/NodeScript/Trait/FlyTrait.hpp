#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	struct TraitFunction final
	{
		NodeTypeID mCallerNodeTypeID;
		NodeTypeID mInputNodeTypeID;
		NodeTypeID mOutputNodeTypeID;
	};

	class Trait final
	{
	public:

		Trait(std::string_view aName);

	public:

		std::string mName;
		std::vector<TraitFunction> mFunctions;
	};
}