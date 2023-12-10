#include "stdafx.h"
#include "Engine/Graphics/Shapes/Pyramid.h"

namespace SU = SimpleUtilities;

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
		{ SU::Vector4f(-1.0f, -1.0f, -1.0f, 1), SU::Vector4f(1.0f, 0.0f, 0.0f, 1.0f),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0.0f, 0.0f) },
		{ SU::Vector4f(1.0f, -1.0f, -1.0f, 1), SU::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1.0f, 0.0f) },
		{ SU::Vector4f(1.0f, -1.0f, 1.0f, 1), SU::Vector4f(0.0f, 0.0f, 1.0f, 1.0f),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1.0f, 1.0f) },
		{ SU::Vector4f(-1.0f, -1.0f, 1.0f, 1), SU::Vector4f(1.0f, 1.0f, 0.0f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0.0f, 1.0f) },

		// Top
		{ SU::Vector4f(0.0f, 1.0f, 0.0f, 1), SU::Vector4f(1.0f, 0.0f, 1.0f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0.5f, 0.5f) }
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

	SU::Matrix4x4f transform;
	transform(4, 1) = 1;
	transform(4, 2) = 0;
	transform(4, 3) = 10;

	const bool success = Model::Init(SimplyGlobal::GetGraphicsEngine(), myVertices, myIndices, transform, "DefaultPS.cso", "DefaultColorfulVS.cso", "Wood.dds");

	return success;
}
