#pragma once
#include "../ScriptDefines.hpp"
#include "../ScriptNodeGraph.hpp"
#include "../Node/NodeRef.hpp"

namespace SCR
{
	class Class;

	class Function final
	{
	public:

		Function(std::string_view aName);
		~Function();

		const std::string& GetName() const
		{
			return mName;
		}

		void SetName(std::string_view aName)
		{
			mName = aName;
		}

		const NodeTypeID& GetCallerNodeTypeID() const
		{
			return mCallerNodeTypeID;
		}

		const NodeTypeID& GetInputNodeTypeID() const
		{
			return mInputNodeTypeID;
		}

		const NodeTypeID& GetOutputNodeTypeID() const
		{
			return mOutputNodeTypeID;
		}

		const NodeID& GetInputNodeID() const
		{
			return mInputNodeID;
		}

		const NodeID& GetOutputNodeID() const
		{
			return mOutputNodeID;
		}

		const NodeRef& GetCaller() const
		{
			return mCaller;
		}

		void SetCaller(const NodeRef& aNodeRef)
		{
			mCaller = aNodeRef;
		}

		NodeGraph& GetNodeGraph()
		{
			return mNodeGraph;
		}

		const NodeGraph& GetNodeGraph() const
		{
			return mNodeGraph;
		}

	private:

		std::string mName;

		NodeTypeID mCallerNodeTypeID;
		NodeTypeID mInputNodeTypeID;
		NodeTypeID mOutputNodeTypeID;

		NodeID mInputNodeID = InvalidID<NodeID>();
		NodeID mOutputNodeID = InvalidID<NodeID>();

		// Temp
		NodeRef mCaller;
		
		NodeGraph mNodeGraph;
	};
}