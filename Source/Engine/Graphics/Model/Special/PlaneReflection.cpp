#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/Special/PlaneReflection.h"
#include "Game/world.hpp"

namespace Simple
{
	PlaneReflection::PlaneReflection()
		: myHeight(0)
	{
		Simple::ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

		myModel = std::move(modelFactory->CreatePlaneReflection());
		myModel->SetScale({ 25,1,25 });
		myModel->SetPosition({ 5,-1,5 });
	}

	PlaneReflection::~PlaneReflection()
	{
	}

	void PlaneReflection::Update()
	{
		myHeight = myModel->GetPosition().y;
		SimpleWorld::SetWaterHeight(myHeight);
	}
}