#pragma once
#include "NodeScript/SimpleScript/Core/Pin/PinType.h"

namespace Editor
{

	class NodeScriptingWindow;

	class FunctionSettingsWindow final
	{
	public:
		FunctionSettingsWindow(NodeScriptingWindow& aParent);
		~FunctionSettingsWindow();
		
		void Update();

	private:

		void ShowInputOutput(SCRIPT::eFlowType aFlowType);

	private:

		NodeScriptingWindow& myParent;
	};

}