#pragma once
#include "../FlyDefines.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Graph/FlyEventGraph.hpp"

namespace FLY_NAMESPACE
{

	struct CPPTraitImplementation final
	{
		NodeTypeID mNodeTypeID;
	};

	struct FlyTraitImplementation final
	{
		EventGraph mNodeGraph;
		NodeTypeID mInputNodeTypeID;
		NodeTypeID mOutputNodeTypeID;

		static constexpr NodeID sInputNodeID{ 0 };
		static constexpr NodeID sOutputNodeID{ 1 };
	};

	using TraitImplementation = std::variant<CPPTraitImplementation, FlyTraitImplementation>;
}