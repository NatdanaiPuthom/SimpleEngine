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

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] NodeTypeID GetCallerNodeTypeID() const;
		[[nodiscard]] NodeTypeID GetInputNodeTypeID() const;
		[[nodiscard]] NodeTypeID GetOutputNodeTypeID() const;
		[[nodiscard]] NodeGraph& GetNodeGraph();
		[[nodiscard]] const NodeGraph& GetNodeGraph() const;

		void SetName(std::string aName);

		static constexpr NodeID INPUT_NODE_ID = NodeID{ 0 };
		static constexpr NodeID OUTPUT_NODE_ID = NodeID{ 1 };

	private:

		std::string mName;

		NodeTypeID mCallerNodeTypeID = InvalidID<NodeTypeID>();
		NodeTypeID mInputNodeTypeID = InvalidID<NodeTypeID>();
		NodeTypeID mOutputNodeTypeID = InvalidID<NodeTypeID>();

		NodeID mInputNodeID = InvalidID<NodeID>();
		NodeID mOutputNodeID = InvalidID<NodeID>();

		NodeGraph mNodeGraph;
	};

	inline NodeTypeID Function::GetCallerNodeTypeID() const
	{
		return mCallerNodeTypeID;
	}
	
	inline NodeTypeID Function::GetInputNodeTypeID() const
	{
		return mInputNodeTypeID;
	}

	inline NodeTypeID Function::GetOutputNodeTypeID() const
	{
		return mOutputNodeTypeID;
	}

	inline NodeGraph& Function::GetNodeGraph()
	{
		return mNodeGraph;
	}

	inline const NodeGraph& Function::GetNodeGraph() const
	{
		return mNodeGraph;
	}
}