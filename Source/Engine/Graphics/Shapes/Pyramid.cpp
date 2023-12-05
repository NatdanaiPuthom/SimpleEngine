#include "stdafx.h"
#include "Pyramid.h"
#include "global.h"
#include "Texture/Texture.h"

Pyramid::Pyramid()
{
}

Pyramid::~Pyramid()
{
}

const bool Pyramid::Create()
{
	myVertices =
	{
		// Base
		{ SimpleUtilities::Vector4f(- 1.0f, -1.0f, -1.0f, 1), SimpleUtilities::Vector4f(0.0f, 0.0f, 1.0f, 1.0f)},
		{ SimpleUtilities::Vector4f(1.0f, -1.0f, -1.0f, 1), SimpleUtilities::Vector4f(0.0f, 0.0f, 1.0f, 1.0f) },
		{ SimpleUtilities::Vector4f(1.0f, -1.0f, 1.0f, 1), SimpleUtilities::Vector4f(0.0f, 0.0f, 1.0f, 1.0f) },
		{ SimpleUtilities::Vector4f(- 1.0f, -1.0f, 1.0f, 1), SimpleUtilities::Vector4f(0.0f, 0.0f, 1.0f, 1.0f)},

		// Top
		{ SimpleUtilities::Vector4f(0.0f, 1.0f, 0.0f, 1), SimpleUtilities::Vector4f(1.0f, 0.0f, 0.0f, 1.0f) }
	};

	myIndices =
	{
		// Base
		0, 1, 2,
		2, 3, 0,

		// Sides
		0, 4, 1,
		1, 4, 2,
		2, 4, 3,
		3, 4, 0
	};

	SimpleUtilities::Matrix4x4f transform;
	transform(4, 1) = 1;
	transform(4, 2) = 0;
	transform(4, 3) = 10;

	const bool success = Model::Init(SimplyGlobal::GetGraphicsEngine(), myVertices, myIndices, transform, eShaderType::Default);

	return success;
}
