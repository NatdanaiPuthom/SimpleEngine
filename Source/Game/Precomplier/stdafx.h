#pragma once
#pragma message("---------Never give up on your dreams!---------------")

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

///////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <External/dearimgui/imgui/imgui.h>
#include <External/dearimgui/imgui/imgui_impl_dx11.h>
#include <External/dearimgui/imgui/imgui_impl_win32.h>
#include <External/dearimgui/imnodes/imnodes.h>
#include <External/nlohmann/json.hpp>
#include <External/profiler.h>

///////////////////////////////////////////////////////////////////////////////////

#include "Engine/SimpleUtilities/Vector2.hpp"
#include "Engine/SimpleUtilities/Vector3.hpp"
#include "Engine/SimpleUtilities/Vector4.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/SimpleUtilities/Matrix4x4.hpp"
#include "Engine/SimpleUtilities/Transform.hpp"
#include "Engine/SimpleUtilities/HashStuff.hpp"
#include "Engine/SimpleUtilities/Ray.hpp"
#include "Engine/SimpleUtilities/Plane.hpp"
#include "Engine/SimpleUtilities/AABB.hpp"
#include "Engine/SimpleUtilities/Intersection.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Input/InputManager.hpp"

#include "Engine/engine.hpp"
#include "Engine/Graphics/GraphicsEngine.hpp"

#include "Engine/Graphics/Camera/Camera.hpp"
#include "Engine/Graphics/Model/ModelInstance.hpp"
#include "Engine/Graphics/Model/ModelFactory.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"

#include "Engine/global.hpp"
#include "Game/world.hpp"

///////////////////////////////////////////////////////////////////////////////////

#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include "Game/Managers/ImGuiManager/Template/ToolInterface.hpp"

///////////////////////////////////////////////////////////////////////////////////

namespace SU = SimpleUtilities;
