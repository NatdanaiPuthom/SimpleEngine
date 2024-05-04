#pragma once
#include "SimpleScript/Core/ScriptDefines.h"

namespace EDIT
{
	class VisualScriptingWindow;

	class NodeCreatorWindow
	{
	public:
		NodeCreatorWindow(VisualScriptingWindow& aParentWindow);
		~NodeCreatorWindow();

		void Update();

	private:
		void EditInputs(SCRIPT::CustomEventID anID);
	private:
		VisualScriptingWindow& myParentWindow;
	};
}