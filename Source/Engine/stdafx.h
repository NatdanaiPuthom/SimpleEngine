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
#include "Engine/Console/Console.h"
#endif

#include <External/profiler.h>

#include "Engine/SimpleUtilities/Vector.h"
#include "Engine/SimpleUtilities/Matrix4x4.h"

#include "Engine/Input/InputManager.h"

#include "Engine/MemoryTracker/MemoryTracker.h"

#include "Engine/engine.h"
#include "Engine/global.h"
#include "Engine/Graphics/GraphicsEngine.h"
#include "Engine/Graphics/Buffers/ConstantBuffer.h"
#include "Engine/Graphics/Shaders/Shader.h"
#include "Engine/Graphics/Model/Model.h"
