#include "stdafx.h"
#include "Engine/global.h"
#include "Engine/engine.h"

namespace
{
	Engine* globalEngine = nullptr;
}

void SimplyGlobalImpl::SetEngine(Engine* aEngine)
{
	globalEngine = aEngine;
}

double SimplyGlobal::GetTotalTime()
{
	return globalEngine->GetTotalTime();
}

float SimplyGlobal::GetDeltaTime()
{
	return globalEngine->GetDeltaTime();
}

HWND& SimplyGlobal::GetHWND()
{
	return globalEngine->GetHWND();
}

GraphicsEngine* SimplyGlobal::GetGraphicsEngine()
{
	return globalEngine->GetGraphicsEngine();
}

