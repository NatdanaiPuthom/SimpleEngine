#pragma once
#include "../FlyDefines.hpp"
#include "../Node/FlyNode.hpp"
#include "../Pin/FlyPin.hpp"
#include "../Pin/FlyLink.hpp"
#include "../Memory/FlyMemoryArena.hpp"

namespace FLY_NAMESPACE
{

	constexpr size_t NodeGraphBufferSize = 1024;

	class NodeGraph final
	{
	public:

		NodeGraph() = default;
		~NodeGraph();

		NodeGraph(const NodeGraph&);
		NodeGraph(NodeGraph&&) noexcept = default;

		NodeGraph& operator=(const NodeGraph&);
		NodeGraph& operator=(NodeGraph&&) noexcept = default;


		[[nodiscard]] Node& GetNode(NodeID aNodeID)
		{
			return mNodes[aNodeID];
		}

		[[nodiscard]] const Node& GetNode(NodeID aNodeID) const
		{
			return mNodes[aNodeID];
		}
		
		[[nodiscard]] size_t GetNodeCount() const
		{
			return mNodes.size();
		}

		[[nodiscard]] const std::vector<Node>& GetNodes() const
		{
			return mNodes;
		}

		[[nodiscard]] NodeID GetCurrentNodeID() const
		{
			return NodeID{ static_cast<NodeID::value_type>(GetNodeCount()) };
		}

		NodeID AddNode(Node aNode)
		{
			NodeID id = GetCurrentNodeID();
			mNodes.push_back(std::move(aNode));
			return id;
		}

		[[nodiscard]] Pin& GetPin(PinID aPinID)
		{
			return mPins[aPinID];
		}

		[[nodiscard]] const Pin& GetPin(PinID aPinID) const
		{
			return mPins[aPinID];
		}

		[[nodiscard]] size_t GetPinCount() const
		{
			return mPins.size();
		}

		[[nodiscard]] PinID AddPin(Pin aPin)
		{
			PinID id{ static_cast<PinID::value_type>(mPins.size()) };
			mPins.push_back(std::move(aPin));
			return id;
		}

		[[nodiscard]] Link& GetLink(LinkID aLinkID)
		{
			return mLinks[aLinkID];
		}

		[[nodiscard]] const Link& GetLink(LinkID aLinkID) const
		{
			return mLinks[aLinkID];
		}

		[[nodiscard]] const std::vector<Link>& GetLinks() const
		{
			return mLinks;
		}

		[[nodiscard]] size_t GetLinkCount() const
		{
			return mLinks.size();
		}

		[[nodiscard]] LinkID AddLink(Link aLink)
		{
			const LinkID id{ static_cast<LinkID::value_type>(mLinks.size()) };
			mLinks.push_back(std::move(aLink));
			return id;
		}

		[[nodiscard]] MemoryArena<NodeGraphBufferSize>& GetMemoryArena()
		{
			return mMemoryArena;
		}

		[[nodiscard]] const MemoryArena<NodeGraphBufferSize>& GetMemoryArena() const
		{
			return mMemoryArena;
		}

	private:


		std::vector<Node> mNodes;
		std::vector<Pin> mPins;
		std::vector<Link> mLinks;

		MemoryArena<NodeGraphBufferSize> mMemoryArena;
	};
}