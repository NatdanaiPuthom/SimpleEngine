#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include <vector>
#include <string>
#include <shellapi.h>

namespace Editor
{
	class FileManager final
	{
	public:
		static std::string myStaticCurrentDirectory;
		static bool myStaticHasDragDropBegin;

	public:
		static void DropFiles(HDROP aHDROP);
		static bool IsFolder(const std::string& aFileName);
		static void ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName);
		static void DrawFilesInFolder(const std::string& aDirectory);

		static const std::string GetFileExtension(const std::string& aFilePath);
		static const std::string GetFileName(const std::string& aFilePath);
		static std::vector<std::string> GetFileNamesFromDirectory(const std::string& aPath, const bool aIncludeFolders = false);
	};
}