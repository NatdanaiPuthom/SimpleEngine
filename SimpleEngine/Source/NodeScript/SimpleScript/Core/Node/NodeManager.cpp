#include "NodeManager.hpp"
#include "NodeTypeManager.hpp"

namespace SCR
{

	NodeManager::NodeManager()
	{
		mNodes.reserve(10);
	}

	NodeManager::~NodeManager()
	{
	}
}
