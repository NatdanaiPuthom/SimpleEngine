#include "stdafx.h"
#include "Cube.h"
#include "global.h"

Cube::Cube()
{
}

Cube::~Cube()
{
}

bool Cube::Create()
{
	myVertices =
	{
		{ SimpleUtilities::Vector4f(-1.0f, 1.0f, -1.0f, 1),  SimpleUtilities::Vector4f(1.0f, 0.0f, 0.0f, 1.0f)},

		{ SimpleUtilities::Vector4f(1.0f, 1.0f, -1.0f, 1),  SimpleUtilities::Vector4f(0.0f, 1.0f, 0.0f, 1.0f)},

		{ SimpleUtilities::Vector4f(-1.0f, -1.0f, -1.0f, 1),  SimpleUtilities::Vector4f(0.0f, 0.0f, 1.0f, 1.0f)} ,

		{ SimpleUtilities::Vector4f(1.0f, -1.0f, -1.0f , 1),  SimpleUtilities::Vector4f(1.0f, 0.0f, 1.0f, 1.0f)},

		{  SimpleUtilities::Vector4f(-1.0f, 1.0f, 1.0f , 1),  SimpleUtilities::Vector4f(0.0f, 1.0f, 1.0f, 1.0f)},

		{  SimpleUtilities::Vector4f(1.0f, 1.0f, 1.0f , 1),  SimpleUtilities::Vector4f(1.0f, 0.0f, 1.0f, 1.0f) },

		{  SimpleUtilities::Vector4f(-1.0f, -1.0f, 1.0f , 1),  SimpleUtilities::Vector4f(1.0f, 1.0f, 0.0f, 1.0f)} ,

		{  SimpleUtilities::Vector4f(1.0f, -1.0f, 1.0f , 1),  SimpleUtilities::Vector4f(1.0f, 1.0f, 1.0f, 1.0f) },

	};

	myIndices =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
	};

	SimpleUtilities::Matrix4x4f transform;
	transform(4, 1) = 0;
	transform(4, 2) = 0;
	transform(4, 3) = 10;
	bool success = Model::Init(SimplyGlobal::GetGraphicsEngine(), myVertices, myIndices, transform, eShaderType::Colorful);
	return success;
}
