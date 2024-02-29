#pragma once
#include "Engine/Threadpool/ThreadPool.hpp"

struct NatdanaiAnimationTest
{
	Simple::AnimatedModel animatedModel;
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
		ThreadPool myThreadPool;
		std::vector< NatdanaiAnimationTest> myThreadTest;
		std::vector<std::future<bool>> myFutureResults;

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

		float myFixedUpdateTimer;
		const float myFixedUpdateFreqency;
	};
}