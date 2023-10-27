#pragma once
#include "Model/Model.h"


class Cube : public Model
{
public:
	Cube();
	~Cube();

	bool Create();
private:
};





























//#include "../Engine/tge/drawers/DebugDrawer.h"
//#include <vector>
//
//#include "Camera2.h"
//#include "Vector.h"
//
//class Cube
//{
//public:
//	Cube(const CommonUtilities::Vector3f& aMin, const CommonUtilities::Vector3f aMax);
//	~Cube() = default;
//
//	void Update(const float aTimeDelta);
//	void Render(Tga::DebugDrawer& aDbg, Camera2& aCamera);
//private:
//	std::vector<CommonUtilities::Vector4<float>> myRotatedCubesCorners;
//	std::vector<CommonUtilities::Vector4<float>> myCubesCorners;
//
//	CommonUtilities::Matrix4x4<float> myModelToWorldTransform;
//
//	CommonUtilities::Vector3f myMin;
//	CommonUtilities::Vector3f myMax;
//
//	CommonUtilities::Vector2f myResolution;
//
//	float myRotateSpeed;
//};