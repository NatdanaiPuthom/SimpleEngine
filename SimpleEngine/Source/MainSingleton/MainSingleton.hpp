#pragma once
#include "MainSingleton/Input/InputManager.hpp"

class MainSingleton final
{
public:
	static Simpleton::InputManager& GetInputManager();
};