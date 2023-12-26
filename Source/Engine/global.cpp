#include "Engine/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

namespace
{
	SimpleUtilities::Vector2ui localResolution = { 800, 600 };
	SimpleUtilities::Vector2ui localWindowSize = { 800, 600 };

	Engine* localEngine = nullptr;
	Renderer* localRenderer = nullptr;
	ModelFactory* localModelFactory = nullptr;

	const float localUpdatePeriodically = 0.5f;

	float localTime = 0;

	int localFramesPerSecond = 0;
	int localFrames = 0;
	int localDrawCalls = 0;

	bool localGameIsRunning = true;
}

void SimpleGlobalEngineImpl::SetEngine(Engine* aEngine)
{
	localEngine = aEngine;
}

void SimpleGlobalEngineImpl::UpdateFPSCounter()
{
	localTime += localEngine->GetDeltaTime();
	++localFrames;

	if (localTime >= localUpdatePeriodically)
	{
		localFramesPerSecond = static_cast<int>(localFrames / localTime);
		localFrames = 0;
		localTime = 0;
	}
}

void SimpleGlobalEngineImpl::ResetDrawCalls()
{
	localDrawCalls = 0;
}

void SimpleGlobalModelFactoryImpl::SetModelFactory(ModelFactory* aModelFactory)
{
	localModelFactory = aModelFactory;
}

void SimpleGlobalRendererImpl::SetRenderer(Renderer* aRenderer)
{
	localRenderer = aRenderer;
}

void SimpleGlobalRendererImpl::IncreaseDrawCall()
{
	++localDrawCalls;
}

namespace SimpleGlobal
{
	Engine* GetEngine()
	{
		return localEngine;
	}

	GraphicsEngine* GetGraphicsEngine()
	{
		return localEngine->GetGraphicsEngine();
	}

	Renderer* GetRenderer()
	{
		return localRenderer;
	}

	ModelFactory* GetModelFactory()
	{
		return localModelFactory;
	}

	HWND& GetHWND()
	{
		return localEngine->GetHWND();
	}

	SimpleUtilities::Vector2ui GetResolution()
	{
		return localResolution;
	}

	SimpleUtilities::Vector2ui GetWindowSize()
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
		return localGameIsRunning;
	}

	void SetGameIsRunning(const bool aShouldRun)
	{
		localGameIsRunning = aShouldRun;
	}

	void SetResolution(const SimpleUtilities::Vector2ui& aResolution)
	{
		localResolution = aResolution;
	}

	void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize)
	{
		localWindowSize = aWindowSize;
	}
}
