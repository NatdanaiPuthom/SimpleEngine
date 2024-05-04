#pragma once
#include "SimpleScript/Core/ScriptDefines.h"


namespace EDIT
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