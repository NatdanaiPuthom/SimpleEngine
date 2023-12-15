#pragma once
//TO-DO: Fix A Way To Not Need to Include These
#include <Windows.h>
#include <d3d11.h>
////

#include <iostream>
#include <cassert>
#include <algorithm>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#ifndef _SIMPLE
#include "Engine/Console/Console.hpp"
#endif

#include "Engine/SimpleUtilities/Vector.hpp"
#include "Engine/SimpleUtilities/Matrix4x4.hpp"

#include "Engine/global.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"

#include <External/profiler.h>
