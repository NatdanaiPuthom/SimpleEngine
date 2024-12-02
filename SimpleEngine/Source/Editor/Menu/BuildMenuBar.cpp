#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/BuildMenuBar.hpp"
#include <filesystem>

namespace Editor
{
	BuildMenuBar::BuildMenuBar()
	{
	}

	BuildMenuBar::~BuildMenuBar()
	{
	}

	void BuildMenuBar::Init()
	{
	}

	void BuildMenuBar::Update()
	{
		//if (ImGui::BeginMainMenuBar())
		//{
		//	if (ImGui::BeginMenu("Build##BuildMenuBar"))
		//	{
		//		if (ImGui::MenuItem("Release##BuildMenuBar"))
		//		{
		//			std::cout << "Build starting..." << std::endl;

		//			const char* premakeCommand = "C:\\Users\\panda\\Desktop\\SimpleEngine\\SimpleEngine\\Premake\\premake5.exe --file=C:\\Users\\panda\\Desktop\\SimpleEngine\\SimpleEngine\\premake5.lua vs2022"; //NOTE(v11.4.2): hardcoded value for testing purposes
		//			int result = system(premakeCommand);

		//			if (result == 0)
		//			{
		//				const std::string stringPath = SimpleUtilities::GetAbsolutePath("Test");
		//				const std::wstring widePath = std::wstring(stringPath.begin(), stringPath.end());
		//				WCHAR szPath[MAX_PATH];
		//				wcsncpy_s(szPath, widePath.c_str(), MAX_PATH - 1);
		//				szPath[MAX_PATH - 1] = L'\0';
		//				CreateDirectory(szPath, NULL);

		//				const char* msbuildCommand = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe\" C:\\Users\\panda\\Desktop\\SimpleEngine\\SimpleEngine\\SimpleEngine.sln /p:Configuration=Release /p:OutputPath=C:\\Users\\panda\\Desktop\\SimpleEngine\\SimpleEngine\\Bin\\Test > C:\\Users\\panda\\Desktop\\SimpleEngine\\SimpleEngine\\Bin\\Test\\msbuild_log.txt 2>&1";
		//				//const char* msbuildCommand = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe\" C:\\Users\\panda\\Desktop\\SimpleEngine\\SimpleEngine\\SimpleEngine.sln /p:Configuration=Release";
		//				result = system(msbuildCommand);

		//				if (result == 0)
		//				{
		//					std::cout << "Build process succeeded!\n";

		//					try
		//					{
		//						std::filesystem::path source = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_ASSETS);
		//						std::filesystem::path destination = SimpleUtilities::GetAbsolutePath("Test/Assets/");
		//						std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive);
		//					}
		//					catch (std::filesystem::filesystem_error& error)
		//					{
		//						std::cout << "Error: " << error.what() << std::endl;
		//					}
		//				}
		//				else
		//				{
		//					std::cerr << "Build process failed with error code: " << result << "\n";
		//				}

		//			}
		//			else
		//			{
		//				std::cerr << "Premake process failed with error code: " << result << "\n";
		//			}
		//		}

		//		ImGui::EndMenu();
		//	}

		//	ImGui::EndMainMenuBar();
		//}
	}

	void BuildMenuBar::Draw()
	{

	}
}
