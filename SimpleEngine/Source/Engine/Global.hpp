#pragma once
#include "Graphics/GraphicsEngine.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"

namespace Global
{
	Simple::GraphicsEngine* GetGraphicsEngine();
	Simple::ModelFactory* GetModelFactory();
	Drawer::Renderer* GetRenderer();

	HWND& GetEngineHWND();

	Math::Vector2ui GetResolution();
	Math::Vector2ui GetWindowSize();

	double GetTotalTime();
	float GetDeltaTime();
	int GetDrawCalls();
	int GetFPS();
	bool GetGameIsRunning();

	void SetGameShouldClose(const bool aShouldClose);
	void SetResolution(const Math::Vector2ui& aResolution);
	void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen = false);
}