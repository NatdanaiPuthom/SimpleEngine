#pragma once
#pragma message("---------Never give up on your dreams!---------------")

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX

#include <Windows.h>

#pragma message("---------Never give up on your dreams!---------------")

#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#pragma message("---------Never give up on your dreams!---------------")

#include <External/profiler.h>

#pragma message("---------Never give up on your dreams!---------------")

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"

#include "Engine/Global.hpp"

namespace SU = SimpleUtilities;
namespace S = Simple;