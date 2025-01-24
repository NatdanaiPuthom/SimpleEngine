#pragma once
#include "Editor/Core/PopUp.hpp"
#include <string>

namespace Editor
{
	class ActiveWindowData;
	class NodeScriptingWindow;

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
		ActiveWindowData* myActiveWindowData;
	public:
		NodeScriptingWindow* myNodeScriptingWindow;
	};
}
