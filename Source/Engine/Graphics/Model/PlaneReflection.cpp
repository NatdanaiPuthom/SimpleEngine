#include "Engine/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"

PlaneReflection::PlaneReflection()
{
	myModelInstance = std::move(SimpleGlobal::GetModelFactory()->CreatePlaneModel());
	myModelInstance->ClearTextures();
	myModelInstance->SetName("Reflection");
	myModelInstance->SetScale({ 5,5,5 });
	myModelInstance->SetPosition({ 10,-1,-5 });
}

PlaneReflection::~PlaneReflection()
{
}
