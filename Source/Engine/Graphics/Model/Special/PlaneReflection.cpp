#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/Special/PlaneReflection.h"
namespace Simple
{
	PlaneReflection::PlaneReflection()
	{
		myModel = std::move(SimpleGlobal::GetModelFactory()->CreatePlaneReflection());
		myModel->SetScale({ 25,1,25 });
		myModel->SetPosition({ 5,-1,5 });
	}

	PlaneReflection::~PlaneReflection()
	{
	}

	void PlaneReflection::Update()
	{
		myHeight = myModel->GetPosition().y;
	}
}