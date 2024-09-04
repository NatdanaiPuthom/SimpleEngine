#pragma once
#include "NodeScript/SimpleScript/Core/FlyDefines.hpp"
#include "NodeScript/SimpleScript/Core/View/FlyVariableView.hpp"

namespace Editor
{
	class NodeScriptingWindow;

	class VariableWindow
	{
	public:
		VariableWindow(NodeScriptingWindow& aParentWindow);

		void Update();

	private:

		void ModifyVariablePopup(Fly::VariableView aVariableView);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}