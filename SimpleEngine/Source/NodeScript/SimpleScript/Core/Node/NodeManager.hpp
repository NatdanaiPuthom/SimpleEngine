#pragma once
#include "../ScriptDefines.hpp"
#include "Node.hpp"
#include <vector>

namespace SCR
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