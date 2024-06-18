#include "MainSingleton/MainSingleton.hpp"

using namespace Simpleton;

void MainSingleton::Init()
{
	AudioManager::GetInstance().Init();
}

void MainSingleton::Destroy()
{
	ECS::ComponentRegistry::GetInstance()->Destroy();
}

InputManager& MainSingleton::GetInputManager()
{
	return InputManager::GetInstance();
}

AudioManager& MainSingleton::GetAudioManager()
{
	return AudioManager::GetInstance();
}

SceneManager& MainSingleton::GetSceneManager()
{
	return SceneManager::GetInstance();
}

ECS::ComponentRegistry* MainSingleton::GetComponentRegistry()
{
	return ECS::ComponentRegistry::GetInstance();
}