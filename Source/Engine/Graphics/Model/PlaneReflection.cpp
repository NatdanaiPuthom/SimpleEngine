#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"

PlaneReflection::PlaneReflection()
{
	myModel = std::move(SimpleGlobal::GetModelFactory()->CreatePlaneReflection());
	myModel->SetScale({ 25,1,25 });
	myModel->SetPosition({ 5,-1,5 });
}

PlaneReflection::~PlaneReflection()
{
}