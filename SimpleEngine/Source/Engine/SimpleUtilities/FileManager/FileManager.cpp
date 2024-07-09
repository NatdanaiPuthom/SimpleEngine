#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Engine/Debugger/Console/Console.hpp"

namespace SimpleUtilities
{
	std::string FileManager::myStaticCurrentDirectory = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS);

	void FileManager::DropFiles(HDROP aHDROP)
	{
		char filePath[MAX_PATH]{};

		const UINT count = DragQueryFileA(aHDROP, 0, NULL, 0);

		for (UINT i = 0; i < count; ++i)
		{
			if (DragQueryFileA(aHDROP, i, filePath, MAX_PATH))
			{
				const std::string name = GetFileName(filePath);
				const std::string destinationPath = myStaticCurrentDirectory + "\\" + name;

				if (CopyFileA(filePath, destinationPath.c_str(), TRUE))
				{
					Simple::Console::Print("File ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been copied to ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(myStaticCurrentDirectory.c_str(), Simple::ConsoleTextColor::Green, true);
				}
				else
				{
					Simple::Console::Print("File ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(name.c_str(), Simple::ConsoleTextColor::Red, false);
					Simple::Console::Print(" already exist at ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(myStaticCurrentDirectory.c_str(), Simple::ConsoleTextColor::Red, true);
				}
			}
		}
	}

	bool FileManager::IsFolder(const std::string& aFileName)
	{
		return (std::string::npos == aFileName.find_last_of('.'));
	}

	const std::string FileManager::GetFileExtension(const std::string& aFilePath)
	{
		for (size_t i = aFilePath.length(); i > 0; i--)
		{
			if (aFilePath[i] == '.')
			{
				return aFilePath.substr(i, aFilePath.length());
			}

			if (aFilePath[i] == '\\')
			{
				break;
			}
		}

		return aFilePath;
	}

	const std::string FileManager::GetFileName(const std::string& aFilePath)
	{
		for (size_t i = aFilePath.length(); i > 0; i--)
		{
			if (aFilePath[i] == '\\')
			{
				return aFilePath.substr(i + 1, aFilePath.length());
			}
		}

		return aFilePath;
	}

	const std::vector<std::string> FileManager::GetFileNamesFromDirectory(const std::string& aAbsolutePath, const bool aIncludeFolders)
	{
		std::vector<std::string> fileNames;

		for (const auto& entry : std::filesystem::directory_iterator(aAbsolutePath))
		{
			if (!aIncludeFolders && !entry.is_regular_file())
			{
				continue;
			}

			fileNames.push_back(entry.path().filename().string());
		}

		return fileNames;
	}

	const std::vector<std::string> FileManager::GetAbsoluteFilePathsFromDirectory(const std::string& aAbsolutePath, const bool aIncludeFolders)
	{
		std::vector<std::string> fileNames;

		for (const auto& entry : std::filesystem::directory_iterator(aAbsolutePath))
		{
			if (!aIncludeFolders && !entry.is_regular_file())
			{
				continue;
			}

			fileNames.push_back(entry.path().string());
		}

		return fileNames;
	}
}