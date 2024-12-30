#pragma once
#include "../FlyDefines.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Node/FlyNodeRef.hpp"

namespace FLY_NAMESPACE
{

	class Function final
	{
	public:

		Function(std::string_view aName);
		~Function();

	public:

		std::string mName;

		NodeTypeID mCallerNodeTypeID = InvalidID<NodeTypeID>();
		NodeTypeID mInputNodeTypeID = InvalidID<NodeTypeID>();
		NodeTypeID mOutputNodeTypeID = InvalidID<NodeTypeID>();

		NodeID mInputNodeID = InvalidID<NodeID>();
		NodeID mOutputNodeID = InvalidID<NodeID>();

		NodeGraph mNodeGraph;
	};
}