#pragma once

namespace Simple
{
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
		std::unique_ptr<Simple::LevelManager> myLevelManager;
		std::unique_ptr<Simple::RaycastManager> myRaycastManager;
		std::unique_ptr<Simple::EventManager> myEventManager;

		float myFixedUpdateTimer;
		const float myFixedUpdateFreqency;
	};
}