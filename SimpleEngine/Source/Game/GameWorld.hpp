#pragma once

struct NatdanaiAnimationTest
{
	Simple::Model model;
	Simple::Animation animation;
	Simple::AnimationPlayer animationPlayer;
};

namespace Simple
{
	class ImGuiManager;
	class LevelManager;
	class RaycastManager;
	class EventManager;
}

namespace Simple
{
	class GameWorld final
	{
	public:
		GameWorld();
		~GameWorld();

		void Init();
		void Update();
		void Render();

	private:
		void NormalUpdate();
		void EarlyUpdate();
		void LateUpdate();
		void FixedUpdate();
	private:
		std::unique_ptr<Simple::ImGuiManager> myImGuiManager;
		std::unique_ptr<Simple::LevelManager> myLevelManager;
		std::unique_ptr<Simple::RaycastManager> myRaycastManager;
		std::unique_ptr<Simple::EventManager> myEventManager;

		NatdanaiAnimationTest myAnimationTest;

		float myFixedUpdateTimer;
		const float myFixedUpdateFreqency;
	};
}