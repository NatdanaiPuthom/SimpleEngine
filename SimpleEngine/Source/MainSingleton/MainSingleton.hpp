#pragma once
#include "MainSingleton/Input/InputManager.hpp"
#include "MainSingleton/Audio/AudioManager.h"

class MainSingleton final
{
public:
	static void Init();

	static Simpleton::InputManager& GetInputManager();
	static Simpleton::AudioManager& GetAudioManager();
};