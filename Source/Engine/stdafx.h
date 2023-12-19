#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
using Microsoft::WRL::ComPtr;

#include <iostream>
#include <cassert>
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#ifndef _SIMPLE
#include "Engine/Console/Console.hpp"
#endif

#include <External/profiler.h>

#include "Engine/SimpleUtilities/Vector.hpp"
#include "Engine/SimpleUtilities/Matrix4x4.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/SimpleUtilities/Transform.hpp"

#include "Engine/Input/InputManager.hpp"

#include "Engine/engine.hpp"
#include "Engine/global.hpp"
#include "Engine/Graphics/Buffers/ConstantBuffer.hpp"
#include "Engine/Graphics/GraphicsEngine.hpp"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Engine/Graphics/Shaders/Shader.hpp"
#include "Engine/Graphics/Model/Mesh.hpp"
#include "Engine/Graphics/Texture/Texture.hpp"
