#pragma once
#include "NodeScript/SimpleScript/Core/ScriptFoundation.h"
#include "NodeScript/SimpleScriptEditor/VisualScriptingWindow.h"
#include "NodeScript/SimpleScript/Core/Serialization/ScriptLoader.h"
#include "NodeScript/SimpleScript/SimpleGameNodes.h"

namespace Script
{
	class SimpleNodeScript final
	{
	public:
		SimpleNodeScript();
		~SimpleNodeScript();

		void Init();
		void Update();

		EDIT::VisualScriptingWindow mySimpleScriptWindow;
		SCRIPT::ScriptFoundation myScriptFoundation;
		SCRIPT::ScriptManager* myScriptManager;
		std::string myName;
	};
}