#pragma once
#include "NodeScript/Fly.hpp"


namespace Editor
{

	class NodeScriptingWindow;

	class StructCreatorWindow final
	{
	public:

		StructCreatorWindow(NodeScriptingWindow* aParentWindow);


		void Update();

		void SetStructFacade(Fly::StructFacade aStructFacade)
		{
			myStructFacade = aStructFacade;
		}
	private:

		void ShowStructMember(Fly::VariableFacade aVariable);

	private:

		NodeScriptingWindow* myParentWindow;
		Fly::StructFacade myStructFacade;

	};
}