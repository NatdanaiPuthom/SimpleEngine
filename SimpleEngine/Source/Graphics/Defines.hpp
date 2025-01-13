#pragma once

namespace Graphics
{
	constexpr size_t Global_Max_Joints = 64;
	constexpr size_t Global_Max_PointLights = 100;

	constexpr size_t Global_Constant_Buffer_Slot_Camera = 0;
	constexpr size_t Global_Constant_Buffer_Slot_Transform = 1;
	constexpr size_t Global_Constant_Buffer_Slot_Time = 2;
	constexpr size_t Global_Constant_Buffer_Slot_Light = 3;
	constexpr size_t Global_Constant_Buffer_Slot_Joints = 4;
	constexpr size_t Global_Constant_Buffer_Slot_PostProcess = 5;
	constexpr size_t Global_Constant_Buffer_Slot_Pointlight = 6;

	constexpr size_t Global_StartSlot_GBuffer = 5;
	constexpr size_t Global_GBuffer_Count = 5;

	constexpr size_t Global_Slot_Albedo = 0;
	constexpr size_t Global_Slot_Normal = 1;
	constexpr size_t Global_Slot_Material = 2;
	constexpr size_t Global_Slot_CubeMap = 4;
	constexpr size_t Global_Slot_ShadowMap = 5;
}