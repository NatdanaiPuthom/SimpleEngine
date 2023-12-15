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
#include "Engine/Console/Console.h"
#endif

#include "Engine/SimpleUtilities/Vector.h"
#include "Engine/SimpleUtilities/Matrix4x4.h"

#include "Engine/global.h"
#include "Engine/Input/InputManager.h"
#include "Engine/SimpleUtilities/Utility.h"

#include <External/profiler.h>
