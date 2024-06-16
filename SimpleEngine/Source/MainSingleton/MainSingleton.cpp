#include "MainSingleton/MainSingleton.hpp"

using namespace Simpleton;

void MainSingleton::Init()
{
	AudioManager::GetInstance().Init();
}

InputManager& MainSingleton::GetInputManager()
{
	return InputManager::GetInstance();
}

AudioManager& MainSingleton::GetAudioManager()
{
	return AudioManager::GetInstance();
}

Simpleton::SceneManager& MainSingleton::GetSceneManager()
{
	return SceneManager::GetInstance();
}