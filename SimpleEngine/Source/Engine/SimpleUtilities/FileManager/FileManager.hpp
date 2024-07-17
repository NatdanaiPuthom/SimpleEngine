#pragma once
#include "External/nlohmann/json.hpp"
#include <vector>
#include <string>
#include <shellapi.h>

namespace ECS
{
	struct ComponentProperty;
}

namespace SimpleUtilities
{
	class FileManager final
	{
	public:
		static std::string myStaticCurrentDirectory;
	public:
		static void DropFiles(HDROP aHDROP);
		static bool IsFolder(const std::string& aFileName);

		static const std::string GetFileExtension(const std::string& aFilePath);
		static const std::string GetFileName(const std::string& aFilePath);
		static const std::vector<std::string> GetFileNamesFromDirectory(const std::string& aAbsolutePath, const bool aIncludeFolders = false);
		static const std::vector<std::string> GetAbsoluteFilePathsFromDirectory(const std::string& aAbsolutePath, const bool aIncludeFolders = false);
		static const nlohmann::json GetDataAsJson(const std::string& aAbsolutePath);

		static const bool IsJSONDataDifferentFromPropertyData(const std::vector<ECS::ComponentProperty>& aComponentProperties, nlohmann::json& aPropertyJson);
	};
}