#include "Game/Precomplied/GamePch.hpp"
#include "AnimationSceneTest.hpp"

namespace Scenes
{
	Scenes::AnimationSceneTest::AnimationSceneTest()
	{
	}

	Scenes::AnimationSceneTest::~AnimationSceneTest()
	{
	}

	void Scenes::AnimationSceneTest::Init()
	{
		myAnimationThreadTest.Init();
	}

	void Scenes::AnimationSceneTest::Update()
	{
		myAnimationThreadTest.Update();
	}

	void Scenes::AnimationSceneTest::Render()
	{
		myAnimationThreadTest.Render();
	}
}