#pragma once

namespace Fly
{
	class VariableFacade;
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
		void ModifyVariablePopup(Fly::VariableFacade& aVariableFacade);

	private:

		NodeScriptingWindow& myParentWindow;
	};
}