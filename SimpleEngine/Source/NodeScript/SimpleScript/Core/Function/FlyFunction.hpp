#pragma once
#include "../FlyDefines.hpp"
#include "../FlyNodeGraph.hpp"
#include "../Node/FlyNodeRef.hpp"

namespace FLY_NAMESPACE
{
	class Class;

	class Function final
	{
	public:

		Function(std::string_view aName);
		~Function();

	public:

		std::string mName;

		NodeTypeID mCallerNodeTypeID;
		NodeTypeID mInputNodeTypeID;
		NodeTypeID mOutputNodeTypeID;

		NodeID mInputNodeID = InvalidID<NodeID>();
		NodeID mOutputNodeID = InvalidID<NodeID>();

		NodeGraph mNodeGraph;
	};
}