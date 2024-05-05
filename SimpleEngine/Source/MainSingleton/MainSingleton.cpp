#include "MainSingleton/MainSingleton.hpp"
#include "Editor/FileManager/FileManager.hpp"

using namespace Simpleton;

void MainSingleton::Init()
{
	AudioManager::GetInstance().Init();
}

void MainSingleton::Release()
{
	Editor::FileManager::Release();

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