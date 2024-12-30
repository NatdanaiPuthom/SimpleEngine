#pragma once
#include "../FlyDefines.hpp"
#include "../Graph/FlyNodeGraph.hpp"

namespace FLY_NAMESPACE
{
	class TraitImplementation final
	{
	public:

		TraitImplementation() = default;

	private:

		NodeGraph mNodeGraph;
	};
}