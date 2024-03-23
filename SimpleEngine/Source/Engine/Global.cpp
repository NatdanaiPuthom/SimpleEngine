#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Engine.hpp"
#include <atomic>

namespace
{
	Math::Vector2ui localResolution = { 800, 600 };
	Math::Vector2ui localWindowSize = { 800, 600 };

	Simple::Engine* localEngine = nullptr;
	Simple::GraphicsEngine* localGraphicsEngine = nullptr;

	const float localUpdatePeriodically = 0.2f;

	float localFPSCounterTime = 0;

	int localFramesPerSecond = 0;
	int localFrames = 0;
	int localDrawCalls = 0;

	bool localIsFullScreen = false;
	std::atomic<bool> localShouldClose = false;
}

namespace Impl
{
	void SimpleGlobalGraphics::UpdateFPSCounter()
	{
		localFPSCounterTime += localEngine->GetDeltaTime();
		++localFrames;

		if (localFPSCounterTime >= localUpdatePeriodically)
		{
			localFramesPerSecond = static_cast<int>(localFrames / localFPSCounterTime);
			localFrames = 0;
			localFPSCounterTime = 0;
		}
	}

	void SimpleGlobalGraphics::ResetDrawCalls()
	{
		localDrawCalls = 0;
	}

	void SimpleGlobalRenderer::IncreaseDrawCall()
	{
		++localDrawCalls;
	}

	void SimpleGlobalEngine::SetEngine(Simple::Engine* aEngine)
	{
		if (localEngine != nullptr)
			assert(false && "localEngine is already set. Is this call a mistake?");

		localEngine = aEngine;
	}

	void SimpleGlobalGraphics::SetGraphicsEngine(Simple::GraphicsEngine* aGraphicsEngine)
	{
		if (localGraphicsEngine != nullptr)
			assert(false && "localGraphicsEngine is already set. Is this call a mistake?");

		localGraphicsEngine = aGraphicsEngine;
	}

	void SimpleGlobalEngine::SetResolution(const Math::Vector2ui& aResolution)
	{
		localResolution = aResolution;
	}

	void SimpleGlobalEngine::SetWindowSize(const Math::Vector2ui& aWindowSize)
	{
		localWindowSize = aWindowSize;
	}
}

namespace Global
{
	Simple::GraphicsEngine* GetGraphicsEngine()
	{
		return localGraphicsEngine;
	}

	Drawer::Renderer* GetRenderer()
	{
		return localGraphicsEngine->GetRenderer();
	}

	Simple::ModelFactory* GetModelFactory()
	{
		return localGraphicsEngine->GetModelFactory();
	}

	HWND& GetEngineHWND()
	{
		return localEngine->GetEngineHWND();
	}

	HCURSOR& GetCustomCursor()
	{
		return localEngine->GetCustomCursor();
	}

	Math::Vector2ui GetResolution()
	{
		return localResolution;
	}

	Math::Vector2ui GetWindowSize()
	{
		return localWindowSize;
	}

	double GetTotalTime()
	{
		return localEngine->GetTotalTime();
	}

	float GetDeltaTime()
	{
		return localEngine->GetDeltaTime();
	}

	int GetDrawCalls()
	{
		return localDrawCalls;
	}

	int GetFPS()
	{
		return localFramesPerSecond;
	}

	bool GetGameIsRunning()
	{
		return !localShouldClose.load();
	}

	bool IsFullScreen()
	{
		return localIsFullScreen;
	}

	void SetGameShouldClose(const bool aShouldClose)
	{
		localShouldClose.store(aShouldClose);
	}

	void SetResolution(const Math::Vector2ui& aResolution)
	{
		localResolution = aResolution;
		localGraphicsEngine->GetCurrentCamera()->UpdateResolution();
	}

	void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen)
	{
		localWindowSize = aWindowSize;
		localResolution = aWindowSize;

		localIsFullScreen = aSetFullScreen;

		localGraphicsEngine->SetWindowSize(localWindowSize, aSetFullScreen);
	}
}