#include "MainSingleton/MainSingleton.hpp"

void MainSingleton::Init()
{
	GetAudioManager().Init();
	GetSceneManager().Init();
}

void MainSingleton::Destroy()
{
	GetSceneManager().Destroy();
	ECS::ComponentRegistry::GetInstance()->Destroy();
}

Simpleton::InputManager& MainSingleton::GetInputManager()
{
	static Simpleton::InputManager inputManager;
	return inputManager;
}

Simpleton::AudioManager& MainSingleton::GetAudioManager()
{
	static Simpleton::AudioManager audioManager;
	return audioManager;
}

Simpleton::SceneManager& MainSingleton::GetSceneManager()
{
	static Simpleton::SceneManager sceneManager;
	return sceneManager;
}

ECS::ComponentRegistry* MainSingleton::GetComponentRegistry()
{
	return ECS::ComponentRegistry::GetInstance();
}