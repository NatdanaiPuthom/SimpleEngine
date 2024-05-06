#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include <vector>
#include <string>
#include <Windows.h>

namespace Editor
{
	class FileManager final
	{
	public:
		inline static std::string sCurrentDirectory = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS);

	public:
		static void Release();

		static void DropFiles(HDROP aHDROP);
		static bool IsFolder(const std::string& aFileName);
		static void ViewFolders(const std::string& aStartDirectory, const std::string& aWindowName);
		static void DrawFilesInFolder(const std::string& aDirectory);

		static const std::string GetFileExtension(const std::string& aFilePath);
		static const std::string GetFileName(const std::string& aFilePath);
		static std::vector<std::string> GetFileNamesFromDirectory(const std::string& aPath, const bool aIncludeFolders = false);
	};
}