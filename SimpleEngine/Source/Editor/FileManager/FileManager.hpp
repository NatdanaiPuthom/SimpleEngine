#pragma once
#include <vector>
#include <string>
#include <Windows.h>

namespace Editor
{
	class FileManager final
	{
	public:
		static void DropFiles(HDROP aHDROP);
		static bool IsFolder(const std::string& aFileName);
		static void ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName, std::string& aCurrentDirectory);

		static const std::string GetFileExtension(const std::string& aFilePath);
		static const std::string GetFileName(const std::string& aFilePath);

		static std::vector<std::string> GetFileNamesFromDirectory(const std::string& aPath, const bool aIncludeFolders = false);
	};
}