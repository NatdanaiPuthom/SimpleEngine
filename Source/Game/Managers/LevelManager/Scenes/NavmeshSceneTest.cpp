#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/LevelManager/Scenes/NavmeshSceneTest.hpp"

Scenes::NavmeshSceneTest::NavmeshSceneTest()
{
}

Scenes::NavmeshSceneTest::~NavmeshSceneTest()
{
}

void Scenes::NavmeshSceneTest::Init()
{
	SimpleWorld::GetEventmanager()->GetPostMaster().AddObserver(this, Simple::eEvent::Raycast);
}

void Scenes::NavmeshSceneTest::Update()
{
}

void Scenes::NavmeshSceneTest::Render()
{
}

void Scenes::NavmeshSceneTest::ReceiveMessage(const Simple::Message<Simple::eEvent>& aMessage)
{
	switch (aMessage.myType)
	{
	case Simple::eEvent::Raycast:
	{
		SU::Vector3f hitPoint = std::any_cast<SU::Vector3f>(aMessage.myData);
		std::cout << "hello world " << hitPoint << std::endl;
		break;
	}
	default:
		break;
	}
}
