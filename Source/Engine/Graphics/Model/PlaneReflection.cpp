#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"

PlaneReflection::PlaneReflection()
{
	myModelInstance = std::move(SimpleGlobal::GetModelFactory()->CreatePlaneModel());
	myModelInstance->ClearTextures();
	myModelInstance->SetName("Reflection");
	myModelInstance->SetScale({ 25,1,25 });
	myModelInstance->SetPosition({ 5,-1,5 });
}

PlaneReflection::~PlaneReflection()
{
}