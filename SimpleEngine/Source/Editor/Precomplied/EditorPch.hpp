#pragma once
#pragma message("---------Never give up on your dreams!---------------")

#ifdef _DEBUG
#include "Engine/Debugger/MemoryTracker/MemoryTracker.h"
#endif

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
#include <filesystem>

#pragma message("---------Never give up on your dreams!---------------")

#include "External/imgui.h"
#include <External/dearimgui/imnodes/imnodes.h>
#include <External/nlohmann/json.hpp>
#include <External/profiler.h>

#pragma message("---------Never give up on your dreams!---------------")

#include "MainSingleton/MainSingleton.hpp"

#pragma message("---------Never give up on your dreams!---------------")

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Math.hpp"

#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"

#include "Engine/Global.hpp"

#pragma message("---------Never give up on your dreams!---------------")

#include "Editor/Template/ToolInterface.hpp"

#pragma message("---------Never give up on your dreams!---------------")

#include "Engine/ECS/Reflection/ECSRegistry.hpp"