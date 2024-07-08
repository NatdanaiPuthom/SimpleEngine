#pragma once
#include "Graphics/GraphicsEngine.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Engine/Math/Vector2.hpp"
#include <Windows.h>
#include <unordered_map>
#include <string>

namespace Global
{
	//TO-DO(v9.27): Seperate nessesary stuff that needed from GraphicsEngine instead of using the GraphicsEngine pointer itself
	Graphics::GraphicsEngine* GetGraphicsEngine(); //NOTES(v9.27): Like Engine class, we should never have direct access to GraphicsEngine but will fix it one day... i hope
	Graphics::ModelFactory* GetModelFactory();
	Drawer::Renderer* GetRenderer();

	HWND& GetEngineHWND();
	const DWORD GetOriginalWindowStyle();
	const HCURSOR& GetCurrentCustomCursor();
	const std::unordered_map<std::string, const HCURSOR>& GetLoadedCustomCursors();

	Math::Vector2ui GetResolution();
	Math::Vector2ui GetWindowSize();

	double GetTotalTime();
	float GetDeltaTime();
	int GetDrawCalls();
	int GetFPS();
	bool GetGameIsRunning();

	bool IsFullScreen();

	void SetGameShouldClose(const bool aShouldClose);
	void SetCustomCursor(const std::string& aCursorName);

	//WindowSize also change Resolution to fit. TO-DO(v9.0?): Fix dragable window size after setting full screen and revert to windowed
	void SetWindowSizeNextFrame(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen = false);
}