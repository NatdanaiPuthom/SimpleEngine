#pragma once

namespace Fly
{
	class VariableView;
}

namespace Editor
{

	class NodeScriptingWindow;

	class FlyClassWindow final
	{
	public:

		FlyClassWindow(NodeScriptingWindow& aParentWindow);

		void Draw();

	private:

		void DrawGraphSelection();
		void DrawFunctionSelection();



		void DrawVariables();
		void ModifyVariablePopup(Fly::VariableView& aVariableView);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}