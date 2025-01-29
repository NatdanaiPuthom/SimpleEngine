#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class MenuTabWindow;
	class MenuItemPopUp;
	class NodeScriptingWindow;
}

namespace Editor
{
	class AssetBrowserPopUp2 final : public PopUp
	{
	public:
		AssetBrowserPopUp2(const std::string& aName, NodeScriptingWindow* aNodeScriptWindow, MenuTabWindow* aNodeScriptParentTab, MenuItemPopUp* aNodeScriptButton);

		void Render() override;

	private:
		void ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName);
		void DrawFilesInFolder(const std::string& aDirectory);
	private:
		std::string myFilePopUpID;
		std::string myFileToRemove;

		NodeScriptingWindow* myNodeScriptingWindow;
		MenuTabWindow* myNodeScriptParentTab;
		MenuItemPopUp* myNodeScriptButton;

		bool myCanOpenPopup;
	};
}
