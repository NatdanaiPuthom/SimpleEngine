#include "NodeManager.hpp"
#include "NodeTypeManager.hpp"

namespace FLY_NAMESPACE
{

	NodeManager::NodeManager()
	{
		mNodes.reserve(10);
	}

	NodeManager::~NodeManager()
	{
	}
}
