#pragma once
#include "NodeScript/SimpleScript/Core/Pin/PinType.h"

namespace Editor
{

	class VisualScriptingWindow;

	class FunctionSettingsWindow final
	{
	public:
		FunctionSettingsWindow(VisualScriptingWindow& aParent);
		~FunctionSettingsWindow();
		
		void Update();

	private:

		void ShowInputOutput(SCRIPT::eFlowType aFlowType);

	private:

		VisualScriptingWindow& myParent;
	};

}