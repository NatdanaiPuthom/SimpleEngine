#pragma once
#include "SimpleScript/Core/ScriptDefines.h"

namespace Editor
{
	class VisualScriptingWindow;

	class VariableWindow
	{
	public:
		VariableWindow(VisualScriptingWindow& aParentWindow);

		void Update();

	private:
		void ModifyVariablePopup(SCRIPT::VarID aVarID);
	private:
		VisualScriptingWindow& myParentWindow;
	};
}