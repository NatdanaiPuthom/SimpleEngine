#pragma once

namespace Editor
{

	class VisualScriptingWindow;

	class FunctionWindow
	{
	public:

		FunctionWindow(VisualScriptingWindow& aParentWindow);
		~FunctionWindow();

		void Update();


	private:


		VisualScriptingWindow& myParentWindow;
	};
}