#pragma once
#include "NodeScript/SimpleScriptEditor/VisualScriptingWindow.h"
#include "NodeScript/SimpleScript/SimpleGameNodes.h"

namespace SCRIPT
{
	class SimpleNodeScript final
	{
	public:
		SimpleNodeScript();
		~SimpleNodeScript();

		void Init();
		void Update();

		Editor::VisualScriptingWindow mySimpleScriptWindow;
		std::string myName;
	};
}