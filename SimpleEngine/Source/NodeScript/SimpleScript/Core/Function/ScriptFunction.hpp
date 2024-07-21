#pragma once
#include "../ScriptDefines.hpp"
#include "../ScriptNodeGraph.hpp"
#include "../Node/NodeRef.hpp"

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

		// Temp
		//NodeRef mCaller;
		
		NodeGraph mNodeGraph;
	};
}