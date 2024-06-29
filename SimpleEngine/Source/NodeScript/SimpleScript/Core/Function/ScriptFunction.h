#pragma once
#include "../ScriptDefines.h"
#include "../ScriptNodeGraph.h"

namespace SCR
{
	class Script;

	class Function final
	{
	public:

		Function(const std::string& aName);
		~Function();

		const std::string& GetName() const
		{
			return myName;
		}

		void SetName(const std::string& aName)
		{
			myName = aName;
		}

		const NodeTypeID& GetCallerNodeTypeID() const
		{
			return myCallerNodeTypeID;
		}

		const NodeTypeID& GetInputNodeTypeID() const
		{
			return myInputNodeTypeID;
		}

		const NodeTypeID& GetOutputNodeTypeID() const
		{
			return myOutputNodeTypeID;
		}

		const NodeID& GetInputNodeID() const
		{
			return myInputNodeID;
		}

		const NodeID& GetOutputNodeID() const
		{
			return myOutputNodeID;
		}

		struct FunctionCaller
		{
			NodeID nodeID = InvalidID<NodeID>();
			NodeGraph* nodeGraph = nullptr;
		};

		const FunctionCaller& GetCaller() const
		{
			return myCaller;
		}

		void SetCaller(const FunctionCaller& aCaller)
		{
			myCaller = aCaller;
		}

		NodeGraph& GetNodeGraph()
		{
			return myNodeGraph;
		}

		const NodeGraph& GetNodeGraph() const
		{
			return myNodeGraph;
		}

	private:

		std::string myName;

		NodeTypeID myCallerNodeTypeID;
		NodeTypeID myInputNodeTypeID;
		NodeTypeID myOutputNodeTypeID;

		NodeID myInputNodeID = InvalidID<NodeID>();
		NodeID myOutputNodeID = InvalidID<NodeID>();

		// Temp
		FunctionCaller myCaller;
		
		NodeGraph myNodeGraph;
	};
}