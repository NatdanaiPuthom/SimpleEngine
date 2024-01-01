#include "Engine/Precomplier/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

namespace
{
	SimpleUtilities::Vector2ui localResolution = { 800, 600 };
	SimpleUtilities::Vector2ui localWindowSize = { 800, 600 };

	Simple::Engine* localEngine = nullptr;
	Simple::Renderer* localRenderer = nullptr;
	Simple::ModelFactory* localModelFactory = nullptr;
	Simple::AudioManager* localAudioManager = nullptr;

	const float localUpdatePeriodically = 0.5f;

	float localTime = 0;

	int localFramesPerSecond = 0;
	int localFrames = 0;
	int localDrawCalls = 0;

	bool localGameIsRunning = true;
}


void Impl::SimpleGlobalEngine::UpdateFPSCounter()
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

void Impl::SimpleGlobalEngine::ResetDrawCalls()
{
	localDrawCalls = 0;
}

void Impl::SimpleGlobalRenderer::IncreaseDrawCall()
{
	++localDrawCalls;
}

void Impl::SimpleGlobalEngine::SetEngine(Simple::Engine* aEngine)
{
	if (localEngine != nullptr)
		assert(false && "localEngine is already set. Is this call a mistake?");

	localEngine = aEngine;
}

void Impl::SimpleGlobalRenderer::SetRenderer(Simple::Renderer* aRenderer)
{
	if (localRenderer != nullptr)
		assert(false && "localRenderer is already set. Is this call a mistake?");

	localRenderer = aRenderer;
}

void Impl::SimpleGlobalModelFactory::SetModelFactory(Simple::ModelFactory* aModelFactory)
{
	if (localModelFactory != nullptr)
		assert(false && "localModelFactory is already set. Is this call a mistake?");

	localModelFactory = aModelFactory;
}

void Impl::SimpleGlobalAudioManager::SetAudioManager(Simple::AudioManager* aAudioManager)
{
	if (localAudioManager != nullptr)
		assert(false && "AudioManager is already set. Is this call a mistake?");

	localAudioManager = aAudioManager;
}

void Impl::SimpleGlobalEngine::SetResolution(const SimpleUtilities::Vector2ui& aResolution)
{
	localResolution = aResolution;
}

void Impl::SimpleGlobalEngine::SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize)
{
	localWindowSize = aWindowSize;
}

namespace SimpleGlobal
{
	Simple::GraphicsEngine* GetGraphicsEngine()
	{
		return localEngine->GetGraphicsEngine();
	}

	Simple::Renderer* GetRenderer()
	{
		return localRenderer;
	}

	Simple::ModelFactory* GetModelFactory()
	{
		return localModelFactory;
	}

	Simple::AudioManager* GetAudioManager()
	{
		return localAudioManager;
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
		localEngine->GetGraphicsEngine()->GetCamera()->UpdateResolution();
	}

	void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize, const bool aSetFullScreen)
	{
		localWindowSize = aWindowSize;
		localEngine->GetGraphicsEngine()->SetWindowSize(localWindowSize, aSetFullScreen);
	}
}
