#pragma once
#include "../../FlyDefines.hpp"
#include "FlyProxyContextIterator.hpp"
#include "../FlyNodeProxy.hpp"

namespace FLY_NAMESPACE
{

	using NodeProxyIterator = ProxyContextIterator<NodeID, NodeProxy>;
	using NodeProxyIteratorService = ProxyContextIteratorService<NodeID, NodeProxyIterator, NodeProxy>;
}