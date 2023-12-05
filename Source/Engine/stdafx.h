#pragma once
#include <iostream>
#include <cassert>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
using Microsoft::WRL::ComPtr;

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <External/profiler.h>

#include "Engine/MemoryTracker/MemoryTracker.h"

#include "Engine/SimpleUtilities/Vector.h"
#include "Engine/SimpleUtilities/Matrix4x4.h"
#include "Engine/SimpleUtilities/InputManager.h"

#include "Engine/Graphics/GraphicsEngine.h" //TO-DO: Fix dependency (Currently it need to be include before engine.h)
#include "Engine/engine.h"

#include "Engine/Graphics/Buffers/ConstantBuffer.h"
#include "Engine/Graphics/Shaders/Shader.h"
#include "Engine/Graphics/Model/Model.h"
