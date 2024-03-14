#pragma once
#pragma message("---------Never give up on your dreams!---------------")

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#pragma message("---------Never give up on your dreams!---------------")

#include <External/dearimgui/imgui/imgui.h>
#include <External/dearimgui/imnodes/imnodes.h>
#include <External/nlohmann/json.hpp>
#include <External/profiler.h>

#pragma message("---------Never give up on your dreams!---------------")

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Math.hpp"
#include "Engine/Math/Intersection.hpp"
#include "Engine/SimpleUtilities/HashStuff.hpp"
#include "Engine/SimpleUtilities/Ray.hpp"
#include "Engine/SimpleUtilities/Plane.hpp"
#include "Engine/SimpleUtilities/AABB2D.hpp"
#include "Engine/SimpleUtilities/AABB3D.hpp"
#include "Engine/SimpleUtilities/Bounds.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/Input/InputManager.hpp"

#include "Engine/Global.hpp"
#include "Game/World.hpp"

#pragma message("---------Never give up on your dreams!---------------")

#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include "Game/Managers/ImGuiManager/Template/ToolInterface.hpp"
#include "Game/PostMaster/Template/PostMaster.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Game/Player/PlayerState.hpp"

#pragma message("---------Never give up on your dreams!---------------")

namespace SU = SimpleUtilities;
namespace S = Simple;