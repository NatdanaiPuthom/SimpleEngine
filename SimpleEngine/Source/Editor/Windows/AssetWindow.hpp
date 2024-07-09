#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include <string>

namespace Editor
{
	class AssetWindow final : public Editor::ToolInterface
	{
	public:
		AssetWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	private:
		void ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName);
		void DrawFilesInFolder(const std::string& aDirectory);
	private:
		std::string myFilePopUpID;
		std::string myFileToRemove;
	};
}