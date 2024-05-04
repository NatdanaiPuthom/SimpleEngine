#pragma once
#include <Game/GameCommon.h>
#include "GameObjects/GameObjectManager.h"
#include <memory>

namespace SCRIPT
{
	class ScriptManager;
}

struct GameLevel
{
	SCRIPT::ScriptManager* scriptManager;
};