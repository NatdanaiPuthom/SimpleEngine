#include "MainSingleton/MainSingleton.hpp"

using namespace Simpleton;

void MainSingleton::Init()
{
	AudioManager::GetInstance().Init();
}

void MainSingleton::Release()
{
	AudioManager::GetInstance().~AudioManager();
}

InputManager& MainSingleton::GetInputManager()
{
	return InputManager::GetInstance();
}

AudioManager& MainSingleton::GetAudioManager()
{
	return AudioManager::GetInstance();
}