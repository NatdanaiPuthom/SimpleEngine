#include "stdafx.h"
#include "Engine/global.h"

namespace
{
	Engine* localEngine = nullptr;
}

void SimplyGlobalImpl::SetEngine(Engine* aEngine)
{
	localEngine = aEngine;
}

double SimplyGlobal::GetTotalTime()
{
	return localEngine->GetTotalTime();
}

float SimplyGlobal::GetDeltaTime()
{
	return localEngine->GetDeltaTime();
}

HWND& SimplyGlobal::GetHWND()
{
	return localEngine->GetHWND();
}

GraphicsEngine* SimplyGlobal::GetGraphicsEngine()
{
	return localEngine->GetGraphicsEngine();
}

