#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "MainSingleton/Input/InputManager.hpp"
#include "MainSingleton/Audio/AudioManager.h"
#include "MainSingleton/SceneManager/SceneManager.hpp"

class MainSingleton final
{
public:
	static void Init();
	static void Destroy();

	static Simpleton::InputManager& GetInputManager();
	static Simpleton::AudioManager& GetAudioManager();
	static Simpleton::SceneManager& GetSceneManager();
	static ECS::ComponentRegistry* GetComponentRegistry();
};