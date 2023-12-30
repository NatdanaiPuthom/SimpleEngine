#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"

PlaneReflection::PlaneReflection()
{
	myModel = std::move(SimpleGlobal::GetModelFactory()->CreatePlaneModel());
	myModel->ClearTextures();
	myModel->SetName("Reflection");
	myModel->SetScale({ 25,1,25 });
	myModel->SetPosition({ 5,-1,5 });
}

PlaneReflection::~PlaneReflection()
{
}