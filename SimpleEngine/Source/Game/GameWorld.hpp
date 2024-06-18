#pragma once

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
		void LateRender();

	private:
		void NormalUpdate();
		void EarlyUpdate();
		void LateUpdate();
		void FixedUpdate();
	private:
		float myFixedUpdateTimer;
		const float myFixedUpdateFreqency;
	};
}