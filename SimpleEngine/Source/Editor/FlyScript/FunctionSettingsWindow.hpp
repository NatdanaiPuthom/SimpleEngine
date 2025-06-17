#pragma once
#include "NodeScript/Pin/FlyPinType.hpp"

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

		void ShowInputOutput(Fly::eIODirection aIODirection);

	private:

		NodeScriptingWindow& myParent;
	};

}