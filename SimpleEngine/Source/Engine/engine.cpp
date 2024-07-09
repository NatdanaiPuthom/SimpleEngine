#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Engine.hpp"
#include "Engine/SimpleUtilities/Timer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include <External/nlohmann/json.hpp>
#include <fstream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace Simple
{
	Engine::Engine()
		: myCurrentCustomCursor(nullptr)
	{
		myOriginalWindowStyle = {};
		myHWND = {};
	}

	Engine::~Engine()
	{
	}

	void Engine::Init(HINSTANCE& hInstance, const int nCmdShow)
	{
#ifndef _SIMPLE
		myConsole.Init();
#endif

		myTimer = std::make_unique<SimpleUtilities::Timer>();

		CheckAndCopySettingsFiles();
		LoadSettingsFromJson();

		const Math::Vector2ui windowSize = Global::GetWindowSize();

		myHWND = SetupMainWindow(hInstance, windowSize.x, windowSize.y);
		assert(myHWND && "Failed To Create Window");

		ShowWindow(myHWND, nCmdShow);
		UpdateWindow(myHWND);

		LoadCustomCursors();

		MainSingleton::GetInputManager().SetHWND(myHWND);

		myOriginalWindowStyle = GetWindowLong(myHWND, GWL_STYLE);
	}

	void Engine::Update()
	{
		myTimer->Update();

		MainSingleton::GetInputManager().Update();
		MainSingleton::GetAudioManager().Update();
	}

	void Engine::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json& windowSizeJson = json["Game_Settings"]["Window_Size"];
		const nlohmann::json& resolutionJson = json["Game_Settings"]["Resolution"];

		Math::Vector2ui windowSize;
		windowSize.x = windowSizeJson["x"];
		windowSize.y = windowSizeJson["y"];

		Math::Vector2ui resolution;
		resolution.x = resolutionJson["x"];
		resolution.y = resolutionJson["y"];

		Impl::SimpleGlobalEngine::SetResolution(resolution);
		Impl::SimpleGlobalEngine::SetWindowSize(windowSize);
	}

	void Engine::LoadCustomCursors()
	{
		myCustomCursors.emplace("DefaultCursor", LoadCursor(nullptr, IDC_ARROW));
		assert(myCustomCursors["DefaultCursor"] && "Failed to load Custom Cursor");

		const std::vector<std::string> absolutePaths = SimpleUtilities::FileManager::GetAbsoluteFilePathsFromDirectory(SimpleUtilities::CheckAndReturnAsAbsolutePath("Assets\\Cursors"));

		for (const std::string& path : absolutePaths)
		{
			const std::string name = SimpleUtilities::FileManager::GetFileName(path);
			const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(path);
			const std::wstring relativePathW = SimpleUtilities::ToWString(relativePath);

			myCustomCursors.emplace(name, LoadCursorFromFile(relativePathW.c_str()));
			assert(myCustomCursors[name] && "Failed to load Custom Cursor");
		}

		myCurrentCustomCursor = &myCustomCursors["White-Cat.cur"];
	}

	void Engine::CheckAndCopySettingsFiles()
	{
		const std::string binSettings = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SETTINGS);
		const std::string dependenciesSettings = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_DEPENDENCIES_SETTINGS);
		const std::string forceDependenciesSettings = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_DEPENDENCIES_FORCE);

		std::vector<std::string> binSettingsFileNames;
		std::vector<std::string> dependenciesSettingsFileNames;
		std::vector<std::string> forceDependenciesSettingsFileNames;

		for (const auto& entry : std::filesystem::directory_iterator(binSettings))
		{
			if (std::filesystem::is_regular_file(entry.path()))
			{
				binSettingsFileNames.push_back(entry.path().filename().string());
			}
		}

		for (const auto& entry : std::filesystem::directory_iterator(dependenciesSettings))
		{
			if (std::filesystem::is_regular_file(entry.path()))
			{
				dependenciesSettingsFileNames.push_back(entry.path().filename().string());
			}
		}

		for (const auto& entry : std::filesystem::directory_iterator(forceDependenciesSettings))
		{
			if (std::filesystem::is_regular_file(entry.path()))
			{
				forceDependenciesSettingsFileNames.push_back(entry.path().filename().string());
			}
		}

		std::vector<std::string> missingFileNames;

		std::sort(dependenciesSettingsFileNames.begin(), dependenciesSettingsFileNames.end());
		std::sort(binSettingsFileNames.begin(), binSettingsFileNames.end());
		std::set_difference(dependenciesSettingsFileNames.begin(), dependenciesSettingsFileNames.end(), binSettingsFileNames.begin(), binSettingsFileNames.end(), std::inserter(missingFileNames, missingFileNames.begin()));

		for (const std::string& name : missingFileNames)
		{
			const std::string source = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_DEPENDENCIES_SETTINGS) + name;
			const std::string destination = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SETTINGS) + name;
			std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);

			Console::Print("Copied: ", ConsoleTextColor::White, false);
			Console::Print(name.c_str(), ConsoleTextColor::Green, true);
		}

		for (const std::string& name : forceDependenciesSettingsFileNames)
		{
			const std::string source = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_DEPENDENCIES_FORCE) + name;
			const std::string destination = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SETTINGS) + name;
			std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);

			Console::Print("Force copied: ", ConsoleTextColor::White, false);
			Console::Print(name.c_str(), ConsoleTextColor::Green, true);
		}
	}

	HWND Engine::SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight)
	{
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wcex.lpszClassName = L"Natdanai";
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
		wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(101));

		assert(wcex.hCursor && "Failed to load cursor");
		assert(wcex.hIcon != NULL && "Failed to load icon");
		assert(wcex.hIconSm != NULL && "Failed to load small icon");

		wcex.style &= ~CS_DROPSHADOW;

		if (!RegisterClassExW(&wcex))
		{
			MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
			return nullptr;
		}

		RECT wr = {};
		wr.left = 0;
		wr.right = aWidth + wr.left;
		wr.top = 0;
		wr.bottom = aHeight + wr.top;

		AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_ENGINE);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json& engineSettings = json["Engine_Settings"];

		const std::string engineNameAndVersionString = std::string(engineSettings["Name"]) + " " + std::string(engineSettings["Version"]) + " " + std::string(engineSettings["Quote"]);
		const std::wstring engineNameAndVersionWide = std::wstring(engineNameAndVersionString.begin(), engineNameAndVersionString.end());

		WCHAR engineNameAndVersion[MAX_PATH];
		wcsncpy_s(engineNameAndVersion, engineNameAndVersionWide.c_str(), MAX_PATH - 1); //NOTE(v11.0.4): disgusting will in future try to figure a better way when I am smarter
		engineNameAndVersion[MAX_PATH - 1] = L'\0';

		return CreateWindow(
			L"Natdanai",
			engineNameAndVersion,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			wr.right - wr.left,
			wr.bottom - wr.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr);
	}

	void Engine::SetGlobalPointerToThis()
	{
		Impl::SimpleGlobalEngine::SetEngine(this);
	}

	void Engine::SetCustomCursor(const std::string& aCursorName)
	{
		if (myCustomCursors.contains(aCursorName))
		{
			myCurrentCustomCursor = &myCustomCursors[aCursorName];
		}
	}

	float Engine::GetDeltaTime() const
	{
		return myTimer->GetDeltaTime();
	}

	const std::unordered_map<std::string, const HCURSOR>& Engine::GetLoadedCustomCursors() const
	{
		return myCustomCursors;
	}

	double Engine::GetTotalTime() const
	{
		return myTimer->GetTotalTime();
	}

	HWND& Engine::GetEngineHWND()
	{
		return myHWND;
	}

	const HCURSOR& Engine::GetCurrentCustomCursor()
	{
		return *myCurrentCustomCursor;
	}

	const DWORD Engine::GetOriginalWindowStyle() const
	{
		return myOriginalWindowStyle;
	}
}