#pragma once

namespace Graphics
{
	constexpr size_t Global_Max_Joints = 64;

	constexpr size_t Global_Constant_Buffer_Slot_Camera = 0;
	constexpr size_t Global_Constant_Buffer_Slot_Transform = 1;
	constexpr size_t Global_Constant_Buffer_Slot_Time = 2;
	constexpr size_t Global_Constant_Buffer_Slot_Light = 3;
	constexpr size_t Global_Constant_Buffer_Slot_Joints = 4;

	constexpr size_t Global_Slot_Albedo = 0;
	constexpr size_t Global_Slot_Material = 1;
	constexpr size_t Global_Slot_Normal = 2;
	constexpr size_t Global_Slot_AmbientOcclusionAndCustom = 3;
	constexpr size_t Global_Slot_CubeMap = 4;
	constexpr size_t Global_Slot_ShadowMap = 5;
}