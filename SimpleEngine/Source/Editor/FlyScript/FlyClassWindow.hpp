#pragma once

namespace Fly
{
	class VariableProxy;
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
		void ModifyVariablePopup(Fly::VariableProxy& aVariableProxy);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}