#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeGraph.hpp"
#include <unordered_map>
#include <span>
#include <optional>

namespace FLY_NAMESPACE
{
	
	constexpr EventID AutoTickEventID{ 0 };

	class EventGraph final
	{
	public:

		void BindNodeToEvent(NodeID aNodeID);
		void UnbindNodeFromEvent(NodeID aNodeID);

		[[nodiscard]] NodeGraph& GetNodeGraph();
		[[nodiscard]] const NodeGraph& GetNodeGraph() const;

		//[[nodiscard]] std::span<const NodeID> GetNodeIDsByEventID(EventID aEventID) const;
		[[nodiscard]] std::optional<std::span<const NodeID>> GetNodeIDsByEventID(EventID aEventID) const;
	private:

		NodeGraph mNodeGraph;
		std::unordered_map<EventID, std::vector<NodeID>> mEventNodes;
	};

	inline NodeGraph& EventGraph::GetNodeGraph()
	{
		return mNodeGraph;
	}

	inline const NodeGraph& EventGraph::GetNodeGraph() const
	{
		return mNodeGraph;
	}

	/*inline std::span<const NodeID> EventGraph::GetNodeIDsByEventID(const EventID aEventID) const
	{
		const auto& vec = mEventNodes.at(aEventID);
		return std::span<const NodeID>(begin(vec), vec.size());
	}*/

	inline std::optional<std::span<const NodeID>> EventGraph::GetNodeIDsByEventID(const EventID aEventID) const
	{
		auto it = mEventNodes.find(aEventID);
		if (it == end(mEventNodes))
		{
			return std::nullopt;
		}
		return std::span(it->second);
	}
}