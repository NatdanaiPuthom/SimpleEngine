#include "MainSingleton/MainSingleton.hpp"

void MainSingleton::Init()
{
	Simpleton::AudioManager::GetInstance().Init();
	Simpleton::SceneManager::GetInstance().Init();
}

void MainSingleton::Destroy()
{
	Simpleton::SceneManager::GetInstance().Destroy();
	ECS::ComponentRegistry::GetInstance()->Destroy();
}

Simpleton::InputManager& MainSingleton::GetInputManager()
{
	return Simpleton::InputManager::GetInstance();
}

Simpleton::AudioManager& MainSingleton::GetAudioManager()
{
	return Simpleton::AudioManager::GetInstance();
}

Simpleton::SceneManager& MainSingleton::GetSceneManager()
{
	return Simpleton::SceneManager::GetInstance();
}

ECS::ComponentRegistry* MainSingleton::GetComponentRegistry()
{
	return ECS::ComponentRegistry::GetInstance();
}