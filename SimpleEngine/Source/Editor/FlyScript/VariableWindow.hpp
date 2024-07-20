#pragma once
#include "NodeScript/SimpleScript/Core/ScriptDefines.hpp"

namespace SCRIPT
{
	class VariableView;
}
namespace Editor
{
	class NodeScriptingWindow;

	class VariableWindow
	{
	public:
		VariableWindow(NodeScriptingWindow& aParentWindow);

		void Update();

	private:

		void ModifyVariablePopup(const SCRIPT::VariableView& aVariable);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}