#pragma once

//Only For global.cpp

namespace Simple
{
	class Engine;
	class Renderer;
}

class ModelFactory;
class AudioManager;

class SimpleGlobalEngineImpl final
{
	friend class Simple::Engine;
private:
	static void SetEngine(Simple::Engine* aEngine);
	static void SetResolution(const SimpleUtilities::Vector2ui& aResolution);
	static void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize);

	static void UpdateFPSCounter();
	static void ResetDrawCalls();
};

class SimpleGlobalRendererImpl final
{
	friend class Simple::Renderer;
private:
	static void IncreaseDrawCall();
	static void SetRenderer(Simple::Renderer* aRenderer);
};

class SimpleGlobalModelFactoryImpl final
{
	friend class ModelFactory;
private:
	static void SetModelFactory(ModelFactory* aModelFactory);
};

class SimpleGlobalAudioManagerImpl final
{
	friend class AudioManager;
private:
	static void SetAudioManager(AudioManager* aAudioManager);
};