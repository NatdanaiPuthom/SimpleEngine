#include "stdafx.h"
#include "Engine/global.h"
#include "Engine/Graphics/Shapes/Cube.h"

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
        {SimpleUtilities::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 1)}, //0
        {SimpleUtilities::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 0)}, //1
        {SimpleUtilities::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 0)}, //2
        {SimpleUtilities::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 1)}, //3

        //West
        {SimpleUtilities::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 1)}, //4
        {SimpleUtilities::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 0)}, //5
        {SimpleUtilities::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 0)}, //6
        {SimpleUtilities::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 1)}, //7

        //South
        {SimpleUtilities::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 1)}, //8 A
        {SimpleUtilities::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 0)}, //9 B
        {SimpleUtilities::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 0)}, //10 C
        {SimpleUtilities::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 1)}, //11 D

        //East
        {SimpleUtilities::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 1)}, //12
        {SimpleUtilities::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 0)}, //13
        {SimpleUtilities::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 0)}, //14
        {SimpleUtilities::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 1)}, //15

        //Up
        {SimpleUtilities::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 1)}, //16
        {SimpleUtilities::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 0)}, //17
        {SimpleUtilities::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 0)}, //18
        {SimpleUtilities::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 1)}, //19

        //Down
        {SimpleUtilities::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 1)}, //20
        {SimpleUtilities::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(0, 0)}, //21
        {SimpleUtilities::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 0)}, //22
        {SimpleUtilities::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SimpleUtilities::Vector4f(1, 1, 1, 1), SimpleUtilities::Vector2f(1, 1)} //23
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

	SimpleUtilities::Matrix4x4f transform;
	transform(4, 1) = -2;
	transform(4, 2) = 0;
	transform(4, 3) = 10;

	bool success = Model::Init(SimplyGlobal::GetGraphicsEngine(), myVertices, myIndices, transform, "DefaultPS.cso", "DefaultVS.cso", "Wood.dds");
	return success;
}
