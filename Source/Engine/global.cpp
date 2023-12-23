#include "Engine/stdafx.h"
#include "Engine/global.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

namespace
{
	Engine* localEngine = nullptr;

	const float localUpdatePeriodically = 0.5f;

	float localTime = 0;

	int localFramesPerSecond = 0;
	int localFrames = 0;
	int localDrawCalls = 0;

	bool localGameIsRunning = true;
}

void SimpleGlobalImpl::SetEngine(Engine* aEngine)
{
	localEngine = aEngine;
}

void SimpleGlobalImpl::UpdateFPSCounter()
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

namespace SimpleGlobal
{
	GraphicsEngine* GetGraphicsEngine()
	{
		return localEngine->GetGraphicsEngine();
	}

	HWND& GetHWND()
	{
		return localEngine->GetHWND();
	}

	double GetTotalTime()
	{
		return localEngine->GetTotalTime();
	}

	float GetDeltaTime()
	{
		return localEngine->GetDeltaTime();
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
}
