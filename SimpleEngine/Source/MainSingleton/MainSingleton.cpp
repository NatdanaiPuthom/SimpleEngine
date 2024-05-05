#include "MainSingleton/MainSingleton.hpp"

Simpleton::InputManager& MainSingleton::GetInputManager()
{
    return Simpleton::InputManager::GetInstance();
}