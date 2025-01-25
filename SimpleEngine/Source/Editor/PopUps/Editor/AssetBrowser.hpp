#pragma once
#include "Editor/Core/PopUp.hpp"
#include <string>

namespace Editor
{
	class MenuTabWindow;
	class MenuItemPopUp;
	class NodeScriptingWindow;
}

namespace Editor
{
	class AssetBrowserPopUp final : public PopUp
	{
	public:
		AssetBrowserPopUp(const std::string& aName);
		void Render() override;
	private:
		void ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName);
		void DrawFilesInFolder(const std::string& aDirectory);

	private:
		std::string myFilePopUpID;
		std::string myFileToRemove;
		bool myCanOpenPopup = false;
	public:
		NodeScriptingWindow* myNodeScriptingWindow;
		MenuTabWindow* myNodeScriptParentTab;
		MenuItemPopUp* myNodeScriptButton;
	};
}
