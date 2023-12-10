#include "stdafx.h"
#include "Engine/Graphics/Shapes/Cube.h"

namespace SU = SimpleUtilities;

Cube::Cube()
{
}

Cube::~Cube()
{
}

const bool Cube::Create()
{
	myVertices =
	{
		//North
		{SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 1)}, //0
		{SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //1
		{SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //2
		{SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //3

		//West
		{SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 1)}, //4
		{SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //5
		{SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //6
		{SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //7

		//South
		{SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 1)}, //8 A
		{SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 0)}, //9 B
		{SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //10 C
		{SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //11 D

		//East
		{SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 1)}, //12
		{SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //13
		{SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //14
		{SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 1)}, //15

		//Up
		{SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 1)}, //16
		{SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //17
		{SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //18
		{SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //19

		//Down
		{SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 1)}, //20
		{SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 0)}, //21
		{SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 0)}, //22
		{SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 1)} //23
	};

	myIndices =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};

	SU::Matrix4x4f transform;
	transform(4, 1) = -2;
	transform(4, 2) = 0;
	transform(4, 3) = 10;

	bool success = Model::Init(SimplyGlobal::GetGraphicsEngine(), myVertices, myIndices, transform, "DefaultPS.cso", "DefaultVS.cso", "Wood.dds");
	return success;
}
