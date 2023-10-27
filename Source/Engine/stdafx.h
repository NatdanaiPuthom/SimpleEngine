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

#include "MemoryTracker/MemoryTracker.h"

#include "SimpleUtilities/Vector.h"
#include "SimpleUtilities/Matrix4x4.h"
#include "SimpleUtilities/InputManager.h"

#include "Graphics/GraphicsEngine.h"
#include "engine.h"

#include "ConstantBuffer/ConstantBuffer.h"
#include "Shaders/Shader.h"
#include "Model/Model.h"
