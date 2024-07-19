#pragma once

namespace Editor
{

	class NodeScriptingWindow;

	class FunctionWindow
	{
	public:

		FunctionWindow(NodeScriptingWindow& aParentWindow);
		~FunctionWindow();

		void Update();


	private:

		NodeScriptingWindow& myParentWindow;
	};
}