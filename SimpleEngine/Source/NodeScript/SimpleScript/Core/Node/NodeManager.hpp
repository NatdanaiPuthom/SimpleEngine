#pragma once
#include "../ScriptDefines.hpp"
#include "Node.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class Class;

	class NodeManager
	{
		friend class ScriptProxy;
	public:

		NodeManager();
		~NodeManager();

	public:

		std::vector<Node> mNodes;

	};

}