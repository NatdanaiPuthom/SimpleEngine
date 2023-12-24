#pragma once
//TO-DO: Fix a way to not need to have these here and still fast complier time
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>

#ifndef _SIMPLE
#include "Engine/Console/Console.hpp"
#endif
///////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cassert>
#include <algorithm>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <thread>

#include "Engine/SimpleUtilities/Vector.hpp"
#include "Engine/SimpleUtilities/Matrix4x4.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/SimpleUtilities/Transform.hpp"

#include "Engine/global.hpp"
#include "Engine/engine.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Graphics/GraphicsEngine.hpp"
#include "Engine/Graphics/Model/Mesh.hpp"
#include "Engine/Graphics/Model/ModelInstance.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"

#include "Game/Managers/ImGuiManager/ToolInterface.hpp"

#include <External/profiler.h>
#include <External/imgui.h>

