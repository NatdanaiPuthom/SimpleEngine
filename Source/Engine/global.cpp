#include "stdafx.h"
#include "Engine/global.h"

namespace
{
	Engine* localEngine = nullptr;

	float localTime = 0;

	const int localOneSecond = 1;
	int localFramesPerSecond = 0;
	int localFrames = 0;
}

void SimplyGlobalImpl::SetEngine(Engine* aEngine)
{
	localEngine = aEngine;
}

void SimplyGlobalImpl::UpdateFPSCounter()
{
	localTime += localEngine->GetDeltaTime();
	++localFrames;

	if (localTime >= localOneSecond)
	{
		localFramesPerSecond = static_cast<int>(localFrames / localTime);
		localFrames = 0;
		localTime = 0;
	}
}

double SimplyGlobal::GetTotalTime()
{
	return localEngine->GetTotalTime();
}

float SimplyGlobal::GetDeltaTime()
{
	return localEngine->GetDeltaTime();
}

int SimplyGlobal::GetFPS()
{
	return localFramesPerSecond;
}

HWND& SimplyGlobal::GetHWND()
{
	return localEngine->GetHWND();
}

GraphicsEngine* SimplyGlobal::GetGraphicsEngine()
{
	return localEngine->GetGraphicsEngine();
}

