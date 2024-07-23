#pragma once
#include "NodeScript/SimpleScript/Core/Pin/FlyPinType.hpp"

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

		void ShowInputOutput(Fly::eFlowType aFlowType);

	private:

		NodeScriptingWindow& myParent;
	};

}