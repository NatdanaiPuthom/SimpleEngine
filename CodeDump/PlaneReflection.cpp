#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Special/PlaneReflection.h"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Engine/Global.hpp"
#include "Game/world.hpp"

namespace Graphics
{
	PlaneReflection::PlaneReflection()
		: myHeight(0)
	{
		myModel->SetScale({ 25,1,25 });
		myModel->SetPosition({ 5,-1,5 });
	}

	PlaneReflection::~PlaneReflection()
	{
	}

	void PlaneReflection::Update()
	{
		myHeight = myModel->GetPosition().y;
		World::SetWaterHeight(myHeight);
	}
}