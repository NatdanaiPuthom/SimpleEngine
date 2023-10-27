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

	bool success = Model::Init(SimplyGlobal::GetGraphicsEngine(), myVertices, myIndices, transform, SimplyGlobal::GetGraphicsEngine()->GetDevice().Get());

	return success;
}












//#include "tge/engine.h"
//#include "Cube.h"
//#include "CommonUtilities/InputManager.h"
//#include <cmath>
//
//Cube::Cube(const CommonUtilities::Vector3f& aMin, const CommonUtilities::Vector3f aMax)
//	: myRotateSpeed(30.0f)
//	, myMin(aMin)
//	, myMax(aMax)
//{
//	auto& engine = *Tga::Engine::GetInstance();
//	myResolution = { static_cast<float>(engine.GetRenderSize().x), static_cast<float>(engine.GetRenderSize().y) };
//
//	const CommonUtilities::Vector4<float> minCorner = CommonUtilities::Vector4<float>{ aMin.x, aMin.y, aMin.z, 1.0f };
//	const CommonUtilities::Vector4<float> maxCorner = CommonUtilities::Vector4<float>{ aMax.x, aMax.y, aMax.z, 1.0f };
//
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{maxCorner.x, maxCorner.y, maxCorner.z, 1.0f});
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{maxCorner.x, minCorner.y, maxCorner.z, 1.0f});
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{minCorner.x, minCorner.y, maxCorner.z, 1.0f});
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{minCorner.x, maxCorner.y, maxCorner.z, 1.0f});
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{maxCorner.x, maxCorner.y, minCorner.z, 1.0f});
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{maxCorner.x, minCorner.y, minCorner.z, 1.0f});
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{minCorner.x, minCorner.y, minCorner.z, 1.0f});
//	myCubesCorners.push_back(CommonUtilities::Vector4<float>{minCorner.x, maxCorner.y, minCorner.z, 1.0f});
//
//	myRotatedCubesCorners = myCubesCorners;
//
//	myModelToWorldTransform(4, 1) = (aMin.x + aMax.x) / 2.0f;
//	myModelToWorldTransform(4, 2) = (aMin.y + aMax.y) / 2.0f;
//	myModelToWorldTransform(4, 3) = (aMin.z + aMax.z) / 2.0f;
//}
//
//void Cube::Update(const float aTimeDelta) 
//{
//	if (CommonUtilities::InputManager::GetInstance().GetAKeyIsPressed())
//	{
//		CommonUtilities::Matrix4x4f rotationMatrix;
//
//		if (CommonUtilities::InputManager::GetInstance().IsHold(VK_SHIFT))
//		{
//			myRotateSpeed = -(std::abs(myRotateSpeed));
//		}
//		else
//		{
//			myRotateSpeed = std::abs(myRotateSpeed);
//		}
//
//		if (CommonUtilities::InputManager::GetInstance().IsHold('T'))
//		{
//			rotationMatrix *= CommonUtilities::Matrix4x4f::CreateRotationAroundX(myRotateSpeed * 3.14f / 180.f * aTimeDelta);
//		}
//
//		if (CommonUtilities::InputManager::GetInstance().IsHold('G'))
//		{
//			rotationMatrix *= CommonUtilities::Matrix4x4f::CreateRotationAroundY(myRotateSpeed * 3.14f / 180.f * aTimeDelta);
//		}
//
//		if (CommonUtilities::InputManager::GetInstance().IsHold('B'))
//		{
//			rotationMatrix *= CommonUtilities::Matrix4x4f::CreateRotationAroundZ(myRotateSpeed * 3.14f / 180.f * aTimeDelta);
//		}
//
//		CommonUtilities::Vector3f cubeCenter;
//		cubeCenter.x = (myMin.x + myMax.x) / 2.0f;
//		cubeCenter.y = (myMin.y + myMax.y) / 2.0f;
//		cubeCenter.z = (myMin.z + myMax.z) / 2.0f;
//		
//		//Does not work! Help!!!!!!!!!! Only works if myMin = (-1, -1, -1) and myMax = (1,1,1)
//		myModelToWorldTransform *= CommonUtilities::Matrix4x4f::CreateTranslation(-1.0f * cubeCenter) * rotationMatrix * CommonUtilities::Matrix4x4f::CreateTranslation(cubeCenter);
//	}
//}
//
//void Cube::Render(Tga::DebugDrawer& aDbg, Camera2& aCamera)
//{
//	for (int i = 0; i < myCubesCorners.size(); ++i)
//	{
//		myRotatedCubesCorners[i] = myCubesCorners[i] * myModelToWorldTransform;
//		myRotatedCubesCorners[i] = aCamera.WorldToCameraSpace(myRotatedCubesCorners[i]);
//		myRotatedCubesCorners[i] = aCamera.CameraToProjectionSpace(myRotatedCubesCorners[i]);
//	}
//
//	const int edges[][2] = {
//		{0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
//		{4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
//		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting edges
//	};
//
//	for (const auto& edge : edges) //Culling lines that are too close and too far 
//	{
//		if ((myRotatedCubesCorners[edge[0]].z > aCamera.GetNearPlane() &&
//			myRotatedCubesCorners[edge[0]].z < aCamera.GetFarPlane()) &&
//			(myRotatedCubesCorners[edge[1]].z > aCamera.GetNearPlane() &&
//				myRotatedCubesCorners[edge[1]].z < aCamera.GetFarPlane()))
//		{
//			const CommonUtilities::Vector2f index0 = aCamera.ProjectionToPixel(myRotatedCubesCorners[edge[0]]);
//			const CommonUtilities::Vector2f index1 = aCamera.ProjectionToPixel(myRotatedCubesCorners[edge[1]]);
//
//			aDbg.DrawLine(Tga::Vector2f( {index0.x, index0.y}), Tga::Vector2f({index1.x, index1.y }));
//		}
//	}
//}
