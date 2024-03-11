#pragma once
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include "Game/Test/AnimationThreading.hpp"

namespace Scenes
{
	class AnimationSceneTest final : public Simple::Scene 
	{
	public:
		AnimationSceneTest();
		~AnimationSceneTest() override;

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		Simple::AnimationThreadTest myAnimationThreadTest;
	};
}