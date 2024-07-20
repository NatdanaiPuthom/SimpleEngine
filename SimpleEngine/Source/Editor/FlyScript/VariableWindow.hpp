#pragma once
#include "NodeScript/SimpleScript/Core/ScriptDefines.hpp"

namespace Fly
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

		void ModifyVariablePopup(const Fly::VariableView& aVariable);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}