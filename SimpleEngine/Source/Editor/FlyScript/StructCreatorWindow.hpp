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

		void SetStructFacade(Fly::GenericDataTypeFacade aDataTypeFacade)
		{
			myGenericDataTypeFacade = aDataTypeFacade;
		}
	private:

		void ShowStructMember(Fly::VariableFacade aVariable);

	private:

		NodeScriptingWindow* myParentWindow;
		Fly::GenericDataTypeFacade myGenericDataTypeFacade;

	};
}