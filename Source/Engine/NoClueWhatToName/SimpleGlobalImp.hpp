#pragma once

//Only For global.cpp

class Engine;
class Renderer;
class ModelFactory;
class AudioManager;

class SimpleGlobalEngineImpl final
{
	friend class Engine;
private:
	static void SetEngine(Engine* aEngine);
	static void SetResolution(const SimpleUtilities::Vector2ui& aResolution);
	static void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize);

	static void UpdateFPSCounter();
	static void ResetDrawCalls();
};

class SimpleGlobalRendererImpl final
{
	friend class Renderer;
private:
	static void IncreaseDrawCall();
	static void SetRenderer(Renderer* aRenderer);
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