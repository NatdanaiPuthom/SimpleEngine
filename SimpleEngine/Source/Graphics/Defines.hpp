#pragma once

namespace Graphics
{
	constexpr size_t GLOBAL_MAX_JOINTS = 64;

	constexpr size_t GLOBAL_CONSTANT_BUFFER_SLOT_CAMERA = 0;
	constexpr size_t GLOBAL_CONSTANT_BUFFER_SLOT_TRANSFORM = 1;
	constexpr size_t GLOBAL_CONSTANT_BUFFER_SLOT_TIME = 2;
	constexpr size_t GLOBAL_CONSTANT_BUFFER_SLOT_LIGHT = 3;
	constexpr size_t GLOBAL_CONSTANT_BUFFER_SLOT_JOINTS = 4;

	constexpr size_t GLOBAL_SLOT_ALBEDO = 0;
	constexpr size_t GLOBAL_SLOT_MATERIAL = 1;
	constexpr size_t GLOBAL_SLOT_NORMAL = 2;
	constexpr size_t GLOBAL_SLOT_CUBEMAP = 3;
	constexpr size_t GLOBAL_SLOT_SHADOWMAP = 4;
}